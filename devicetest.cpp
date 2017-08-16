#include "devicetest.h"
#include <QSerialPortInfo>
#include <QDebug>

#include "datafilemanager.cpp"

DeviceTest* DeviceTest::instance = nullptr;
DeviceTest::DeviceTest(QObject *parent) : QObject(parent), serialPort(), deviceStatus(CLOSED), dataFilemgr("tmp.txt")
{
    dataFilemgr.start();
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
            QVariantList data = extractRealData(byteBuffer);
            dataFilemgr.writeFile(data);
            emit deviceReadyRead(data);
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

bool DeviceTest::setSerialPort(const QVariant& portName, const QVariantList& portSetting)
{
    qDebug() << "start setSerialPort";
    return false;
}

bool DeviceTest::openSerialPort(const QVariant& portName)
{
    qDebug() << "start openSerialPort";
    serialPort.setPortName(portName.toString());
    serialPort.setBaudRate(QSerialPort::Baud115200);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setParity(QSerialPort::NoParity);

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

QVariantList DeviceTest::extractRealData(QByteArray& buffer)
{
//    qDebug() << "start data extract: " << buffer;

    QVariantList result;
    uchar headFlag1 = 0xaa, headFlag2 = 0x55;
    const int wndSize = 20, dataUnitLen = wndSize - 2;
    uchar oddData[20] = "";

    uchar* wndPtr = (uchar*)buffer.data();

    for (int begin = 0, end = buffer.size() - wndSize; begin < end; ++begin)
    {
        if (wndPtr[dataUnitLen] == headFlag1 && wndPtr[dataUnitLen+1] == headFlag2)
        {
            uchar rawdata[20] = "";
            memcpy_s(rawdata, 20, wndPtr, dataUnitLen);
            for (int pos = 0, end = dataUnitLen / 3; pos < end; ++pos){
               result.append((((uint)rawdata[pos * 3 + 1]) << 8) + (uint)rawdata[pos * 3 + 2]);
            }

            wndPtr += 20;
        }
        else
        {
            wndPtr += 1;
        }
        memcpy_s(oddData, dataUnitLen, wndPtr, dataUnitLen);
    }
    buffer.clear();
    buffer.append((char*)oddData);

    return result;
}

int DeviceTest::openDataFile(const QVariant& fileName)
{
    qDebug() << "start open data file";
    return -1;
}

int DeviceTest::saveExtractedDataToFile(const QVariant& fileName, char* data)
{
    return -1;
}

int DeviceTest::saveDataToFile(const QVariant& filename)
{
    qDebug() << "start save data file";
    return -1;
}
