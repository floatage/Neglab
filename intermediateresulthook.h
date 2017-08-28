#ifndef INTERMEDIATERESULTHOOK_H
#define INTERMEDIATERESULTHOOK_H

#include <QFile>
#include <QString>
#include <QQueue>
#include <QVariant>
#include <QMutex>
#include <QTextStream>

#include "rawdatahandlemanager.h"

//若需其他格式按照此类实现一个类加入到RawDataHandleManager即可
class QThread;
class CSVWriter: public ExecuteObject
{
    Q_OBJECT
public:
    CSVWriter();
    CSVWriter(const QString& iFilename, int iChannelNum);
    ~CSVWriter();

    void init(const QVariant & param);
    void clear();

    void bind(QThread* thread);
    void execute(const QVariant& params);
    int identifier(){return 1;}

private:
    int channelNum;
    QQueue<QVariant> dataQueue;
    QString fileName;
    QFile file;
    QTextStream out;
    QThread* writeThread;
    QMutex lock;
    bool isLoop;

signals:
    void hasDataCanWrite();

public slots:
    void writeData();
};

#endif // INTERMEDIATERESULTHOOK_H
