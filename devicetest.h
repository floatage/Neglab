#ifndef DEVICETEST_H
#define DEVICETEST_H

#include <QObject>
#include <QSerialPort>
#include <QVariant>
#include <QString>
#include <QVector>
#include <QQueue>
#include <QThread>

#include "rawdatahandlemanager.h"

class QSerialPort;
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

    Q_INVOKABLE int startDataTransfer();
    Q_INVOKABLE int pauseDataTransfer();
    Q_INVOKABLE int finishDataTransfer();

    Q_INVOKABLE int openDataFile(const QVariant& fileName);
    Q_INVOKABLE int saveDataToFile(const QVariant& filename);

    Q_INVOKABLE int getChannelNum()const{ return deviceChannelNum; }

    enum DeviceStatus{CLOSED, OPEN, PAUSE, DATATRANSFER};

signals:
    void deviceReadyRead(QVariant readData);
    void deviceJudgeFinished(void);
    void deviceByteBufferFilled(const QVariant& buffer);
    void plotDataReady(QVariant plotData);

public slots:
    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError error);
    void handleGetNextBuffer(QVariant plotData);

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
    RawDataHandleManager* rawDataHandleMgr;
    QThread dataHandleThread;

    int judgeDeviceChannelNum(const QByteArray& data);
    void dataTransferMainProcess(const QByteArray& buffer);
};

#endif // DEVICETEST_H
