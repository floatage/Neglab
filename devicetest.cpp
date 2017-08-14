#include "devicetest.h"

#include <QSerialPortInfo>
#include <QDebug>

DeviceTest* DeviceTest::instance = nullptr;
DeviceTest::DeviceTest(QObject *parent) : QObject(parent), serialPort(), dataTransferIsStart(false)
{
    connect(&serialPort, SIGNAL(readyRead()), SLOT(handleReadyRead()));
    connect(&serialPort, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(handleError(QSerialPort::SerialPortError)));
}

void DeviceTest::handleReadyRead()
{
    buffer.append(serialPort.readAll());

    qDebug() << "handle buffer before" << buffer;

    QStringList lines = buffer.split("\r\n");
    if (lines.size() != 1)
    {
        for (int lineCount = 0, end = lines.size() - 1; lineCount < end; ++lineCount)
        {
            emit deviceReadyRead(lines[lineCount]);
        }
        buffer.swap(lines[lines.size()-1]);
    }
    else if (buffer.size() > 60)
    {
        emit deviceReadyRead(buffer);
        buffer.clear();
    }

    qDebug() << "handle buffer after" << buffer;
}

void DeviceTest::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ReadError) {
        qDebug() << "Read Error";
    }
}

DeviceTest* DeviceTest::getInstance(void)
{
    if (DeviceTest::instance == nullptr){
        DeviceTest::instance = new DeviceTest();
    }

    return DeviceTest::instance;
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

    return serialPort.open(QSerialPort::ReadWrite);;
}

int DeviceTest::sendDataToPort(const QVariant& data)
{
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
    int err = -1, readCounter = 0;
    uchar buf[1100] = "", oddData[20] = "";
    uchar headFlag1 = 0xaa, headFlag2 = 0x55;
    const ushort wndSize = 20, bufSize = 1024;
    const int dataUnitLen = wndSize - 2;
    memset(buf, 0, bufSize);

//    std::string readData;
//    int readlen = m_datmgr->rawread(buf, bufSize);
//    uchar* wndPtr = buf;
//    while (true)
//    {
//        for (int begin = 0, end = readlen - wndSize; begin < end; ++begin)
//        {
//            if (wndPtr[dataUnitLen] == headFlag1 && wndPtr[dataUnitLen+1] == headFlag2)
//            {
//                uchar rawdata[20] = "", pack[12] = "";
//                memcpy_s(rawdata, 20, wndPtr, dataUnitLen);
//                for (int pos = 0, end = dataUnitLen / 3; pos < end; ++pos)
//                    memcpy_s(pack + pos * 2, 2, rawdata + pos * 3 + 1, 2);

//                readData += (char*)pack;
//                negPrintBuffer(pack, 12, 1);
//                wndPtr += 20;
//            }
//            else
//            {
//                wndPtr += 1;
//            }
//            memcpy_s(oddData, dataUnitLen, wndPtr, dataUnitLen);
//        }

//        ++readCounter;
//        if (readCounter % 10 == 0) {
//            storeNegData((char*)readData.c_str(), readData.size());
//            readData.clear();
//        }

//        readlen = m_datmgr->rawread(buf + dataUnitLen, bufSize);
//        memcpy_s(buf, dataUnitLen, oddData, dataUnitLen);
//        readlen += dataUnitLen;
//        wndPtr = buf;
//    }

    return err;
}

int DeviceTest::pauseDataTransfer()
{
    return -1;
}

int DeviceTest::finishDataTransfer()
{
    return -1;
}

char* DeviceTest::extractRealData()
{
    return NULL;
}

int DeviceTest::openDataFile(const QVariant& fileName)
{
    return -1;
}

int DeviceTest::saveExtractedDataToFile(const QVariant& fileName, char* data)
{
    return -1;
}

int DeviceTest::saveDataToFile(const QVariant& filename)
{
    return -1;
}
