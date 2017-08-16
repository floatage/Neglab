#ifndef DEVICETEST_H
#define DEVICETEST_H

#include <QObject>
#include <QSerialPort>
#include <QVariant>
#include <QString>
#include <QVector>

#include "datafilemanager.h"

class QSerialPort;
class DeviceTest : public QObject
{
    Q_OBJECT
public:
    ~DeviceTest();
    static DeviceTest* getInstance(void);

    Q_INVOKABLE QVariantList getAvailPortNames(void);
    Q_INVOKABLE bool setSerialPort(const QVariant& portName, const QVariantList& portSetting);
    Q_INVOKABLE bool openSerialPort(const QVariant& portName);
    Q_INVOKABLE int sendDataToPort(const QVariant& data);
    Q_INVOKABLE QVariant recvDataFromPort(void);
    Q_INVOKABLE bool disconnectPort(void);

    Q_INVOKABLE void searchDevice(void);
    Q_INVOKABLE bool connectDevice(const QVariant& deviceNum);

    Q_INVOKABLE int startDataTransfer();
    Q_INVOKABLE int pauseDataTransfer();
    Q_INVOKABLE int finishDataTransfer();

    Q_INVOKABLE int openDataFile(const QVariant& fileName);
    Q_INVOKABLE int saveDataToFile(const QVariant& filename);

    enum DeviceStatus{CLOSED, OPEN, PAUSE, DATATRANSFER};

signals:
    void deviceReadyRead(QVariant readData);

public slots:
    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError error);

private:
    explicit DeviceTest(QObject *parent = 0);
    DeviceTest(const DeviceTest&);
    DeviceTest& operator =(const DeviceTest&);

    static DeviceTest* instance;

    QSerialPort serialPort;
    QString strBuffer;
    QByteArray byteBuffer;

    DeviceStatus deviceStatus;
    DataFileManager dataFilemgr;


    QVariantList extractRealData(QByteArray& buffer);
    int saveExtractedDataToFile(const QVariant& fileName, char* data);
};

#endif // DEVICETEST_H
