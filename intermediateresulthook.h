#ifndef INTERMEDIATERESULTHOOK_H
#define INTERMEDIATERESULTHOOK_H

#include <QFile>
#include <QString>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QVariant>

#include "rawdatahandlemanager.h"

class CSVWriter: public ExecuteObject
{
public:
    CSVWriter(const QString& iFilename, int iChannelNum);
    ~CSVWriter();

    void init(const QVariant & param);
    void clear();

    void stop();
    void execute(const QVariant& params);
    int identifier(){return 1;}

private:
    int channelNum;
    QQueue<QVariant> dataQueue;
    QString fileName;
    QMutex lock;
    QWaitCondition signal;
    bool canRun;
    QFile file;

    void run();
};

#endif // INTERMEDIATERESULTHOOK_H
