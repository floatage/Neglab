#include "datasourcemanager.h"
#include "devicetest.h"

#include <QString>
#include <QThread>

PlotDataSource* DataSourceManager::getDataSource(const QString &url)
{
    if (url.endsWith(".csv")){
        return new PlotDataSource_CSV(url);
    }

    return NULL;
}

PlotDataSource_CSV::PlotDataSource_CSV(const QString &csvFileName)
    :filename(csvFileName), plotSignalOwner(NULL), file(), textStream(), canRead(true), dataPackLineNumber(25)
{
    file.setFileName(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    textStream.setDevice(&file);
}

PlotDataSource_CSV::~PlotDataSource_CSV()
{
    lock.lock();
    if (file.isOpen()) file.close();
    if (thread) thread->deleteLater();
    lock.unlock();
}

void PlotDataSource_CSV::bind(DeviceTest* plotSignalOwner, QThread* workThread)
{
    if (!plotSignalOwner || !workThread) throw std::exception("PlotDataSource_CSV bind error");

    thread = workThread;
    this->plotSignalOwner = plotSignalOwner;
    this->moveToThread(thread);
    connect(this, &PlotDataSource_CSV::PlotDataGenerated, plotSignalOwner, &DeviceTest::handlePlotDataGenerated);
    connect(plotSignalOwner, &DeviceTest::plotDataTransferFinished, this, &PlotDataSource_CSV::run);
    thread->start();
}

int PlotDataSource_CSV::judgeDeviceChannelNum()
{
    QString title = textStream.readLine();
    QStringList titleColumn = title.split(',');
    return titleColumn.size() - 1;
}

void PlotDataSource_CSV::start()
{
    lock.lock();
    canRead = true;
    lock.unlock();

    run();
}

void PlotDataSource_CSV::stop()
{
    lock.lock();
    canRead = false;
    if (file.isOpen()) file.close();
    lock.unlock();
}

void PlotDataSource_CSV::pause()
{
    lock.lock();
    canRead = false;
    lock.unlock();
}

//此处会造成线程在执行时被中断，但线程内没有资源泄露情况，资源都由本类析构函数释放
void PlotDataSource_CSV::run()
{
    lock.lock();
    if (canRead){
        QVariantList result;
        for (int begin = 0; begin < dataPackLineNumber && !textStream.atEnd(); ++begin){
            QString line = textStream.readLine();
            QVariantList pack;

            foreach (QString channelData, line.split(',')) {
                pack.append(channelData.toFloat());
            }
            result.append(QVariant(pack));
        }

        emit PlotDataGenerated(result);
    }
    lock.unlock();
}
