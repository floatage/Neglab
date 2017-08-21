#include "devicetest.h"
#include <QSerialPortInfo>
#include <QDebug>

DeviceTest* DeviceTest::instance = nullptr;
DeviceTest::DeviceTest(QObject *parent)
    : QObject(parent), serialPort(), deviceStatus(CLOSED), deviceChannelNum(-1),
      packHeadFlag1(0xaa), packHeadFlag2(0x55)
{
    connect(&serialPort, SIGNAL(readyRead()), SLOT(handleReadyRead()));
    connect(&serialPort, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(handleError(QSerialPort::SerialPortError)));
}

DeviceTest* DeviceTest::getInstance(void)
{
    if (DeviceTest::instance == nullptr){
        DeviceTest::instance = new DeviceTest();
    }

    return DeviceTest::instance;
}

DeviceTest::~DeviceTest()
{
    if (DeviceTest::instance)
        delete DeviceTest::instance;
}

void DeviceTest::handleReadyRead()
{
    if (deviceStatus == OPEN)
    {
        strBuffer.append(serialPort.readAll());
        qDebug() << "handle buffer before" << strBuffer;

        QStringList lines = strBuffer.split("\r\n");
        if (lines.size() != 1)
        {
            for (int lineCount = 0, end = lines.size() - 1; lineCount < end; ++lineCount)
            {
                emit deviceReadyRead(lines[lineCount]);
            }
            strBuffer.swap(lines[lines.size()-1]);
        }
        else if (strBuffer.size() > 60)
        {
            emit deviceReadyRead(strBuffer);
            strBuffer.clear();
        }

        qDebug() << "handle buffer after" << strBuffer;
    }
    else if (deviceStatus == PAUSE)
    {

    }
    else if (deviceStatus == DATATRANSFER)
    {
        byteBuffer.append(serialPort.readAll());

        if (byteBuffer.size() > 128){
            QByteArray oddData = dataTransferMainProcess(byteBuffer);
            byteBuffer.clear();
            byteBuffer.append(oddData);
        }
    }
}

void DeviceTest::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ReadError) {
        qDebug() << "Read Error";
    }
}

QVariantList DeviceTest::getAvailPortNames(void)
{
    qDebug() << "start getAvailPortNames";
    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();
    QVariantList portNameList;
    foreach (const QSerialPortInfo& item, portList) {
        portNameList.append(item.portName());
    }

    return portNameList;
}

bool DeviceTest::setupSerialPort(const QVariant& portName, const QVariantList& portSetting)
{
    qDebug() << "start setSerialPort";
    serialPort.setPortName(portName.toString());
    serialPort.setBaudRate(QSerialPort::Baud115200);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setParity(QSerialPort::NoParity);
    return false;
}

bool DeviceTest::openSerialPort(const QVariant& portName)
{
    qDebug() << "start openSerialPort";

    setupSerialPort(portName, QVariantList());
    deviceStatus = OPEN;
    return serialPort.open(QSerialPort::ReadWrite);
}

int DeviceTest::sendDataToPort(const QVariant& data)
{
    if (deviceStatus != OPEN){
        return -1;
    }

    qDebug() << "start sendDataToPort" << data;
    QString writeData = data.toString();
    int writeSize = (int)serialPort.write(writeData.toStdString().c_str());

    if (writeSize == -1){
        qDebug() << "fail write data to serial port";
    }
    else if (writeSize != writeData.size()){
        qDebug() << "fail write all data to serial port";
    }

    return writeSize;
}

QVariant DeviceTest::recvDataFromPort(void)
{
    if (deviceStatus != OPEN){
        return -1;
    }

    qDebug() << "start recvDataFromPort";
    QByteArray readData = serialPort.readAll();
    qDebug() << readData;

    return readData.toStdString().c_str();
}

bool DeviceTest::disconnectPort(void)
{
    qDebug() << "start disconnectPort";
    if (serialPort.isOpen())
        serialPort.close();
    deviceStatus = CLOSED;
    return true;
}

void DeviceTest::searchDevice(void)
{
    qDebug() << "start searchDevice";
    QVariantList portNameList = getAvailPortNames();

    if (portNameList.length() >  0)
    {
        if (openSerialPort(portNameList[0]))
        {
            sendDataToPort(QVariant("AT:GS"));
        }
        else
        {
            qDebug() << "open serial port failed, port name is" << portNameList[0].toString();
        }
    }else
    {
        qDebug() << "not found serial port";
    }
}

bool DeviceTest::connectDevice(const QVariant &deviceNum)
{
    qDebug() << "start connectDevice";
    QString connectStr = QString("AT:GI-#") + deviceNum.toString();
    if (sendDataToPort(connectStr) == connectStr.size()){
        qDebug() << "connect device successed";
        return true;
    }

    qDebug() << "connect device failed";
    return false;
}

int DeviceTest::startDataTransfer()
{
    qDebug() << "start data transfer";
    deviceStatus = DATATRANSFER;
    return 1;
}

int DeviceTest::pauseDataTransfer()
{
    qDebug() << "pause data transfer";
    deviceStatus = PAUSE;
    return 1;
}

int DeviceTest::finishDataTransfer()
{
    qDebug() << "finish data transfer";
    deviceStatus = OPEN;
    return 1;
}

int DeviceTest::judgeDeviceChannelNum(const QByteArray& data)
{
    static std::map<int, int> channelNumMap{
        std::pair<int, int>(18, 2),
        std::pair<int, int>(27, 8),
        std::pair<int, int>(51, 16),
        std::pair<int, int>(99, 32)
    };

    int firstFlagPos = -1;
    for (int begin = 0, end = data.size()-1; begin < end; ++begin){
        if (data[begin] == packHeadFlag1 && data[begin+1] == packHeadFlag2){
            if (firstFlagPos == -1){
                firstFlagPos = begin;
            }else{
                int packLen = begin - firstFlagPos;
                return channelNumMap.count(packLen) > 0 ? channelNumMap[packLen] : -1;
            }
        }
    }

    return -1;
}

QByteArray DeviceTest::dataTransferMainProcess(QByteArray buffer)
{
    if (deviceChannelNum == -1){
        deviceChannelNum = judgeDeviceChannelNum(buffer);
        return QByteArray();
    }

    return QByteArray();
}

int DeviceTest::openDataFile(const QVariant& fileName)
{
    qDebug() << "start open data file";
    return -1;
}

int DeviceTest::saveDataToFile(const QVariant& filename)
{
    qDebug() << "start save data file";
    return -1;
}
