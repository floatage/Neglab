#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "rawdatahandlemanager.h"

#include <QVector>
#include <QByteArray>
#include <unordered_map>

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

    inline bool isArriveHeadFlag(uchar* pos);
    inline void createDataPack(uchar* pos, QVariantList& container);
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
    QVariant buffer;
};

class DataFilter_LowpassFilter: public DataFilter
{
public:
    void handle(QVariant& data);
    int identifier(){return 1;}
};

class DataFilter_HighpassFilter: public DataFilter
{
public:
    void handle(QVariant& data);
    int identifier(){return 2;}
};

class DataCutter_Noraml: public DataCutter
{
public:
    void handle(QVariant& data);
    int identifier(){return 1;}
};

class CSVWriter: public ExecuteObject
{
public:
    void execute(QVariant params);
    int identifier(){return 1;}

private:
    void run();
};

#endif // DATAHANDLER_H
