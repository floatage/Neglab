#ifndef DATAFILEMANAGER_H
#define DATAFILEMANAGER_H

#include <QFile>
#include <QThread>
#include <QString>
#include <QMutex>
#include <QVariant>

class DataFileManager : public QThread
{
public:
    DataFileManager(const QString& filename);
    void writeFile(const QVariant& data);
    void stop();
    ~DataFileManager();

private:
    QVariant buffer;
    QString fileName;
    QMutex signal;
    bool isRuning;
    QFile file;

    void run();
};

#endif // DATAFILEMANAGER_H
