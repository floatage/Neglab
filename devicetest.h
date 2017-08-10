#ifndef DEVICETEST_H
#define DEVICETEST_H

#include <QObject>
#include <QSerialPort>
#include <QVariant>
#include <QString>

class QSerialPort;
class DeviceTest : public QObject
{
    Q_OBJECT
private:
    explicit DeviceTest(QObject *parent = 0);
    DeviceTest(const DeviceTest&);
    DeviceTest& operator =(const DeviceTest&);

    static DeviceTest* instance;

    QSerialPort serialPort;
    QString buffer;

signals:
    void deviceReadyRead(QVariant readData);

public slots:
    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError error);

public:
    static DeviceTest* getInstance(void);

    Q_INVOKABLE QVariantList getAvailPortNames(void);
    Q_INVOKABLE bool setSerialPort(const QVariant& portName, const QVariantList& portSetting);
    Q_INVOKABLE bool openSerialPort(const QVariant& portName);
    Q_INVOKABLE int sendDataToPort(const QVariant& data);
    Q_INVOKABLE QVariant recvDataFromPort(void);
    Q_INVOKABLE bool disconnectPort(void);

    Q_INVOKABLE void searchDevice(void);
    Q_INVOKABLE bool connectDevice(const QVariant& deviceNum);
};

#endif // DEVICETEST_H
