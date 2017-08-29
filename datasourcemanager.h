#ifndef DATASOURCEMANAGER_H
#define DATASOURCEMANAGER_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QFile>
#include <QMutex>
#include <QTextStream>


//绘图数据源接口
class QThread;
class DeviceTest;
class PlotDataSource: public QObject
{
    Q_OBJECT
public:
    virtual int judgeDeviceChannelNum() = 0;
    virtual void bind(DeviceTest* plotSignalOwner, QThread* workThread) = 0;
    virtual void start() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual ~PlotDataSource(){}
};

class PlotDataSource_CSV: public PlotDataSource
{
    Q_OBJECT
public:
    PlotDataSource_CSV(const QString& csvFileName);
    ~PlotDataSource_CSV();

    int judgeDeviceChannelNum();
    void bind(DeviceTest* plotSignalOwner, QThread* workThread);
    void start();
    void pause();
    void stop();
    void run();

private:
    DeviceTest* plotSignalOwner;
    QString filename;
    QFile file;
    QTextStream textStream;
    QMutex lock;
    QThread* thread;

    bool canRead;
    int dataPackLineNumber;

signals:
    void PlotDataGenerated(QVariant plotData);
};

//可实现为可注册数据源的管理类，此处只实现为静态工厂类
class DataSourceManager
{
public:
    static PlotDataSource* getDataSource(const QString& url);
};

#endif // DATASOURCEMANAGER_H
