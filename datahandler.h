#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "commonvariable.h"
#include "rawdatahandlemanager.h"

#include <QVector>
#include <QByteArray>
#include <QVariant>
#include <QMutex>

//数据分包处理组件，为了保证数据完整，分包会将上次剩下的数据添加到新数据开头，保证数据逻辑连续
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
    void createDataPack(uchar* pos, QVariantList& container);
    uint byteToInt(uchar* head, int len);
};

//数据采样组件，为保证正常数据正常采样，若数据没有足够的数据进行采样则存储此次数据供下次采样
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

//数据滤波组件
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
