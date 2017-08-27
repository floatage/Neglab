#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "commonvariable.h"
#include "rawdatahandlemanager.h"

#include <QVector>
#include <QByteArray>
#include <QVariant>
#include <QMutex>

class DataExtracter_RemainHandle: public DataExtracter
{
public:
    DataExtracter_RemainHandle();
    DataExtracter_RemainHandle(int iChannelNum);
    ~DataExtracter_RemainHandle();
    void init(const QVariant& param);
    void handle(QVariant& data);
    int identifier(){return 1;}

private:
    QMutex lock;
    QByteArray remainData;
    int channelNum;
    int controlDataLen;
    int dataLen;
    int packLen;
    int channelDataLen;

//    const QVector<uchar> headFlag;
//    inline bool isArriveHeadFlag(uchar* pos);
    inline void createDataPack(uchar* pos, QVariantList& container);
    inline int byteToInt(uchar* head, int len);
};

class DataSampler_DownSampler: public DataSampler
{
public:
    DataSampler_DownSampler();
    DataSampler_DownSampler(int sampleRate);
    void init(const QVariant& param);
    void handle(QVariant& data);
    int identifier(){return 1;}

private:
    QMutex lock;
    int fetchInterval;
    QVariantList validDataList;
};

class DataFilter_IIR: public DataFilter
{
public:
    DataFilter_IIR();
    DataFilter_IIR(int iBufferLen, int iChannelNum, const QVector<float> &ia, const QVector<float> &ib);
    ~DataFilter_IIR();
    void handle(QVariant& data);
    void init(const QVariant& param);
    void clear();
    int identifier(){return 1;}

private:
    QMutex lock;
    QVector<float> a;
    QVector<float> b;
    QVector<QVector<float>*> channelHistoryList;
    int bufferLen;
    int bufferWritePos;
    int channelNum;
    float gain;
    DataMapFuncPtr dataMapFuncPtr;
};

#endif // DATAHANDLER_H
