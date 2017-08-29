#include "devicetest.h"
#include <QSerialPortInfo>
#include <QVariantList>
#include <QThread>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QVariantMap>

#include "rawdatahandlemanager.h"
#include "datahandler.h"
#include "intermediateresulthook.h"
#include "datasourcemanager.h"

DeviceTest* DeviceTest::instance = nullptr;
DeviceTest::DeviceTest(QObject *parent)
    : QObject(parent), serialPort(), deviceStatus(CLOSED), deviceChannelNum(-1), rawDataHandleMgr(NULL), dataHandleThread(NULL),
      dataSource(NULL)
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

int DeviceTest::startDataTransfer(const QVariant& controlData)
{
    qDebug() << "start data transfer" << controlData.toMap();
    buildHandleComponent();
    deviceStatus = DATATRANSFER;
    dataHandleControlData = controlData;
    if (dataSource) dataSource->start();
    return 1;
}

int DeviceTest::pauseDataTransfer()
{
    qDebug() << "pause data transfer";
    deviceStatus = PAUSE;
    if (dataSource) dataSource->pause();
    return 1;
}

int DeviceTest::finishDataTransfer()
{
    qDebug() << "finish data transfer";
    deviceStatus = CLOSED;
    if (serialPort.isOpen())
        serialPort.close();
    deviceChannelNum = -1;
    byteBufferQueue.clear();

    if (dataSource){
        dataSource->stop();
        delete dataSource;
        dataSource = NULL;
    }

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
    //为了防止信号丢失的情况，达到一定次数重新激活循环
    static int reActiveNumer = CommonVariable::dataHandleLoopReactiveTimes;
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

    qDebug() << QThread::currentThreadId() << "fill queue" << endl;
    byteBufferQueue.push_back(QVariant(byteBuffer));

    if (--reActiveNumer){
        loopIsRun = false;
        reActiveNumer = CommonVariable::dataHandleLoopReactiveTimes;
    }

    if (!loopIsRun){
        loopIsRun = true;
        emit deviceByteBufferFilled(byteBufferQueue.front());
        byteBufferQueue.pop_front();
    }
}

void DeviceTest::handleGetNextBuffer(QVariant plotData)
{
    qDebug() << QThread::currentThreadId() << "out queue" << endl;
    emit plotDataReady(plotData);
    if (!byteBufferQueue.empty()){
        emit deviceByteBufferFilled(byteBufferQueue.front());
        byteBufferQueue.pop_front();
    }
}

void DeviceTest::handlePlotDataGenerated(QVariant plotData)
{
    qDebug() << QThread::currentThreadId() << "data generated" << endl;
    emit plotDataReady(plotData);
    emit plotDataTransferFinished();
}

//构建处理组件
void DeviceTest::buildHandleComponent()
{
    int smapleRate = 100;

    QVariantMap bulidParams = dataHandleControlData.toMap();
    if (bulidParams.contains("sampleRate")) smapleRate = bulidParams["sampleRate"].toInt();

    rawDataHandleMgr = RawDataHandleManager::getInstance();
    rawDataHandleMgr->clear();
    rawDataHandleMgr->addHandler(new DataExtracter_RemainHandle(deviceChannelNum));
    rawDataHandleMgr->addHandler(new DataSampler_DownSampler(smapleRate));

    int historyLen = CommonVariable::historyDataBufferLen;
    rawDataHandleMgr->addHandler(new DataFilter_IIR(historyLen, deviceChannelNum, QVector<float>(historyLen,1.0f), QVector<float>(historyLen,1.0f)));
    rawDataHandleMgr->addIntermediateResultHook(4000, 1, new CSVWriter("filterBefor.csv", deviceChannelNum));
    rawDataHandleMgr->addIntermediateResultHook(10000, 1, new CSVWriter("filterAfter.csv", deviceChannelNum));

//初始化处理组件的保险方法
//    static bool isBuild = false;
//    static DataHandler *de = new DataExtracter_RemainHandle(), *ds = new DataSampler_DownSampler(), *df = new DataFilter_IIR();
//    static ExecuteObject* dsWriter = new CSVWriter();

//    if (!isBuild){
//        isBuild = true;
//        rawDataHandleMgr->addHandler(de);
//        rawDataHandleMgr->addHandler(ds);
//        rawDataHandleMgr->addHandler(df);
//        rawDataHandleMgr->addIntermediateResultHook(4000, 1, dsWriter);
//    }

//    de->init(deviceChannelNum);
//    ds->init(100);

//    QVariantList dfParams;
//    dfParams.append(CommonVariable::historyDataBufferLen);
//    dfParams.append(deviceChannelNum);
//    dfParams.append(QVariant(QVariantList::fromVector(QVector<QVariant>(CommonVariable::historyDataBufferLen, 0.0f))));
//    dfParams.append(QVariant(QVariantList::fromVector(QVector<QVariant>(CommonVariable::historyDataBufferLen, 0.0f))));
//    df->init(dfParams);

//    QVariantList dsWriterParams;
//    dsWriterParams.append("test.csv");
//    dsWriterParams.append(deviceChannelNum);
//    dsWriter->init(dsWriterParams);
}

int DeviceTest::openDataFile(const QVariant& fileName)
{
    qDebug() << "start open data file" << fileName.toString();
    QString filenameStr = fileName.toString();
    filenameStr = filenameStr.right(filenameStr.length() - 8);

    dataSource = DataSourceManager::getDataSource(filenameStr);
    if (dataSource){
        dataSource->bind(this, new QThread());
        deviceChannelNum = dataSource->judgeDeviceChannelNum();
        emit deviceJudgeFinished();
        return 1;
    }

    return -1;
}

int DeviceTest::saveDataToDir(const QVariant& dirName, const QVariant& description)
{
    qDebug() << "start save data file";

    QFile gatherInforFile("gatherInfor.txt");
    gatherInforFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&gatherInforFile);
    out.setCodec("UTF-8");
    QVariantMap gatherInfor = description.toMap();

    for (QVariantMap::iterator it = gatherInfor.begin(); it != gatherInfor.end(); ++it){
        out << it.key() << ": " << it.value().toString() << CommonVariable::newlineStr;
    }
    gatherInforFile.close();

    QString dir = dirName.toString();
    dir = dir.right(dir.length() - 8);

    return CommonVariable::copyCurDirFile(CommonVariable::tmpFileDefaultPath, dir, CommonVariable::dataFilePattern);
}

int DeviceTest::modifyDefaultSetting(const QVariant& newSetting)
{
    return 1;
}
