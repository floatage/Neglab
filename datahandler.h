#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "rawdatahandlemanager.h"

#include <QVector>
#include <QByteArray>
#include <unordered_map>
#include <QVariant>
#include <QFile>
#include <QString>
#include <QMutex>
#include <QWaitCondition>

class DataExtracter_RemainHandle: public DataExtracter
{
public:
    DataExtracter_RemainHandle(int iChannelNum);
    ~DataExtracter_RemainHandle();
    void init(const QVariant& param);
    void handle(QVariant& data);
    int identifier(){return 1;}

private:
    typedef std::unordered_map<int, std::vector<int>> ChannelControlDict;

    int channelNum;
    const QVector<uchar> headFlag;
    QByteArray remainData;

    ChannelControlDict channelControlDict;
    int controlDataLen;
    int dataLen;
    int packLen;
    int channelDataLen;

    inline bool isArriveHeadFlag(uchar* pos);
    inline void createDataPack(uchar* pos, QVariantList& container);
    inline int byteToInt(uchar* head, int len);
};

class DataSampler_DownSampler: public DataSampler
{
public:
    DataSampler_DownSampler(int sampleRate);
    void init(const QVariant& param);
    void handle(QVariant& data);
    int identifier(){return 1;}

private:
    int fetchInterval;
    QVariant remainData;
};

class DataFilter_IIR: public DataFilter
{
public:
    DataFilter_IIR(int iBufferLen, int iChannelNum, const QVector<float> &ia, const QVector<float> &ib);
    ~DataFilter_IIR();
    void handle(QVariant& data);
    int identifier(){return 1;}

private:
    typedef float (*DataMapFuncPtr)(int, float);
    DataMapFuncPtr getMatchDataMapFuncPtr(int channelNum);

    QVector<float> a;
    QVector<float> b;
    QVector<QVector<float>*> channelHistoryList;
    int bufferLen;
    int bufferWritePos;
    int channelNum;
    float gain;
    DataMapFuncPtr dataMapFuncPtr;
};

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
    QVariant buffer;
    QString fileName;
    QMutex lock;
    QWaitCondition signal;
    bool canRun;
    QFile file;

    void run();
};

#endif // DATAHANDLER_H
