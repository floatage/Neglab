#include "datafilemanager.h"
#include <QFile>
#include <QTextStream>

DataFileManager::DataFileManager(const QString& filename)
    : fileName(filename), signal(), buffer(), isRuning(true), file(filename)
{
    file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    signal.lock();
}

DataFileManager::~DataFileManager()
{
    signal.unlock();
    file.close();
}

void DataFileManager::writeFile(const QVariant& data)
{
    buffer.clear();
    buffer = data;
    signal.unlock();
}

void DataFileManager::run()
{
    QTextStream out(&file);
    while(isRuning){
        signal.lock();
        QList<QVariant> dataList = buffer.toList();
        for (int begin = 0; begin < dataList.size(); ++begin){
            out << dataList[begin].toInt() << ',';
        }
        out << '\n';
        out.flush();
        file.flush();
        signal.lock();
    }
}

void DataFileManager::stop()
{
    isRuning = false;
    signal.unlock();
}
