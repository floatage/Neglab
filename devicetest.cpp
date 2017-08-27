#include "devicetest.h"
#include <QSerialPortInfo>
#include <QVariantList>
#include <QThread>
#include <QDebug>

#include "rawdatahandlemanager.h"
#include "datahandler.h"
#include "intermediateresulthook.h"

DeviceTest* DeviceTest::instance = nullptr;
DeviceTest::DeviceTest(QObject *parent)
    : QObject(parent), serialPort(), deviceStatus(CLOSED), deviceChannelNum(-1), rawDataHandleMgr(NULL), dataHandleThread(NULL)
{
    initDataHandleThread();

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

void DeviceTest::initDataHandleThread()
{
    rawDataHandleMgr = RawDataHandleManager::getInstance();
    dataHandleThread = new QThread();
    rawDataHandleMgr->moveToThread(dataHandleThread);
    connect(dataHandleThread, &QThread::finished, rawDataHandleMgr, &QObject::deleteLater);
    connect(this, &DeviceTest::deviceByteBufferFilled, rawDataHandleMgr, &RawDataHandleManager::handleDeviceByteBufferFilled);
    connect(rawDataHandleMgr, &RawDataHandleManager::getNextBuffer, this, &DeviceTest::handleGetNextBuffer);
    dataHandleThread->start();
}

//此处可用状态模式重构
void DeviceTest::handleReadyRead()
{
    if (deviceStatus == OPEN)
    {
        strBuffer.append(serialPort.readAll());
//        qDebug() << "handle buffer before" << strBuffer;

        QStringList lines = strBuffer.split("\r\n");
        if (lines.size() != 1)
        {
            for (int lineCount = 0, end = lines.size() - 1; lineCount < end; ++lineCount)
            {
                emit deviceReadyRead(lines[lineCount]);
            }
            strBuffer.swap(lines[lines.size()-1]);
        }
        else if (strBuffer.size() > CommonVariable::openModeBufferMaxSize)
        {
            emit deviceReadyRead(strBuffer);
            strBuffer.clear();
        }

//        qDebug() << "handle buffer after" << strBuffer;
    }
    else if (deviceStatus == PAUSE)
    {
    }
    else if (deviceStatus == DATATRANSFER)
    {
        byteBuffer.append(serialPort.readAll());

        if (byteBuffer.size() > CommonVariable::dataTransferModeBufferMaxSize){
            dataTransferMainProcess(byteBuffer);
            byteBuffer.clear();
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

void DeviceTest::searchDevice(void)
{
    qDebug() << "start searchDevice";
    QVariantList portNameList = getAvailPortNames();

    if (portNameList.length() >  0)
    {
        if (openSerialPort(portNameList[0]))
        {
            sendDataToPort(QVariant(CommonVariable::searchDeviceCommandStr));
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
    QString connectStr = CommonVariable::connectDeviceCommandStr + deviceNum.toString();
    if (sendDataToPort(connectStr) == connectStr.size()){
        qDebug() << "connect device successed";
        return true;
    }

    qDebug() << "connect device failed";
    return false;
}

bool DeviceTest::disconnectPort(void)
{
    qDebug() << "start disconnectPort";
    deviceStatus = CLOSED;
    if (serialPort.isOpen())
        serialPort.close();
    return true;
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
    deviceStatus = CLOSED;
    if (serialPort.isOpen())
        serialPort.close();
    deviceChannelNum = -1;
    initDataHandleThread();
    rawDataHandleMgr->clear();
    return 1;
}

int DeviceTest::judgeDeviceChannelNum(const QByteArray& data)
{
    uchar* dataPtr = (uchar*)data.data();
    int firstFlagPos = -1;
    for (int begin = 0, end = data.size()-1; begin < end; ++begin){
        if (*(dataPtr+begin) == CommonVariable::packHeadFlag1 && *(dataPtr+begin+1) == CommonVariable::packHeadFlag2){
            if (firstFlagPos == -1){
                firstFlagPos = begin;
            }else{
                int packLen = begin - firstFlagPos;
                return CommonVariable::channelNumMap.count(packLen) > 0 ? CommonVariable::channelNumMap[packLen] : -1;
            }
        }
    }

    return -1;
}

void DeviceTest::dataTransferMainProcess(const QByteArray& buffer)
{
    static bool loopIsRun = false;

    if (deviceChannelNum == -1){
        deviceChannelNum = judgeDeviceChannelNum(buffer);
        if (deviceChannelNum != -1){
            loopIsRun = false;
            buildHandleComponent();

            emit deviceJudgeFinished();
        }

        return;
    }

    qDebug() << "fill queue" << endl;
    byteBufferQueue.push_back(QVariant(byteBuffer));

    if (!loopIsRun){
        loopIsRun = true;
        emit deviceByteBufferFilled(byteBufferQueue.front());
        byteBufferQueue.pop_front();
    }
}

void DeviceTest::buildHandleComponent()
{
    static bool isBuild = false;
    static DataHandler *de = new DataExtracter_RemainHandle(), *ds = new DataSampler_DownSampler(), *df = new DataFilter_IIR();

    if (!isBuild){
        isBuild = true;
        rawDataHandleMgr->addHandler(de);
        rawDataHandleMgr->addHandler(ds);
        rawDataHandleMgr->addHandler(df);
    //    rawDataHandleMgr->addIntermediateResultHook(4000, 1, new CSVWriter("/raw/test.csv", deviceChannelNum));
    //    rawDataHandleMgr->addIntermediateResultHook(10000, 1, new CSVWriter("/filter/test.csv", deviceChannelNum));
    }

    de->init(deviceChannelNum);
    ds->init(100);

    QVariantList dfParams;
    dfParams.append(CommonVariable::historyDataBufferLen);
    dfParams.append(deviceChannelNum);
    dfParams.append(QVariant(QVariantList::fromVector(QVector<QVariant>(CommonVariable::historyDataBufferLen, 0.0f))));
    dfParams.append(QVariant(QVariantList::fromVector(QVector<QVariant>(CommonVariable::historyDataBufferLen, 0.0f))));
    df->init(dfParams);
}

void DeviceTest::handleGetNextBuffer(QVariant plotData)
{
    qDebug() << "out queue" << endl;
    emit plotDataReady(plotData);
    if (!byteBufferQueue.empty()){
        emit deviceByteBufferFilled(byteBufferQueue.front());
        byteBufferQueue.pop_front();
    }
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
