#ifndef DEVICETEST_H
#define DEVICETEST_H

#include <QObject>
#include <QSerialPort>
#include <QVariant>
#include <QString>
#include <QQueue>

#include "commonvariable.h"

class QSerialPort;
class RawDataHandleManager;
class PlotDataSource;
//此类没有设计好，可将设备操作和与上层的绘图接口分离成两个部分，以便可动态切换绘图的数据源
class DeviceTest : public QObject
{
    Q_OBJECT
public:
    ~DeviceTest();
    static DeviceTest* getInstance(void);

    Q_INVOKABLE QVariantList getAvailPortNames(void);
    Q_INVOKABLE bool setupSerialPort(const QVariant& portName, const QVariantList& portSetting);
    Q_INVOKABLE bool openSerialPort(const QVariant& portName);
    Q_INVOKABLE int sendDataToPort(const QVariant& data);
    Q_INVOKABLE QVariant recvDataFromPort(void);

    Q_INVOKABLE void searchDevice(void);
    Q_INVOKABLE bool connectDevice(const QVariant& deviceNum);
    Q_INVOKABLE bool disconnectPort(void);

    Q_INVOKABLE int startDataTransfer(const QVariant& controlData);
    Q_INVOKABLE int pauseDataTransfer();
    Q_INVOKABLE int finishDataTransfer();

    Q_INVOKABLE int openDataFile(const QVariant& fileName);
    Q_INVOKABLE int saveDataToDir(const QVariant& dirName, const QVariant& description);

    Q_INVOKABLE int modifyDefaultSetting(const QVariant& newSetting);

    Q_INVOKABLE int getChannelNum()const{ return deviceChannelNum; }

    enum DeviceStatus{CLOSED, OPEN, PAUSE, DATATRANSFER};

signals:
    //以下均采用值传递，qt的隐式共享可保证效率，否则可用智能指针代替防止多次复制
    void deviceReadyRead(QVariant readData);
    void deviceJudgeFinished(void);
    void deviceByteBufferFilled(const QVariant& buffer);
    void plotDataReady(QVariant plotData);
    void plotDataTransferFinished();

public slots:
    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError error);
    void handleGetNextBuffer(QVariant plotData);
    void handlePlotDataGenerated(QVariant plotData);

private:
    explicit DeviceTest(QObject *parent = 0);
    DeviceTest(const DeviceTest&);
    DeviceTest& operator =(const DeviceTest&);

    static DeviceTest* instance;

    QSerialPort serialPort;
    QString strBuffer;
    QByteArray byteBuffer;
    QQueue<QVariant> byteBufferQueue;

    DeviceStatus deviceStatus;
    int deviceChannelNum;
    QVariant dataHandleControlData;
    RawDataHandleManager* rawDataHandleMgr;
    QThread* dataHandleThread;
    bool handleLoopIsRun;

    PlotDataSource* dataSource;

    void initDataHandleThread();
    void buildHandleComponent();
    int judgeDeviceChannelNum(const QByteArray& data);
    void dataTransferMainProcess(const QByteArray& buffer);
};

#endif // DEVICETEST_H
