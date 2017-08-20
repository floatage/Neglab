#include "datahandler.h"
#include <cstring>
#include <QList>
#include <QVariantList>

DataExtracter_RemainHandle::DataExtracter_RemainHandle(int iChannelNum)
    : headFlag{0xaa, 0x55}, channelControlDict()
{
    channelControlDict[2] = {18, 16, 0};
    channelControlDict[8] = {27, 25, 1};
    channelControlDict[16] = {51, 49, 1};
    channelControlDict[32] = {99, 97, 1};
    init(iChannelNum);
}

DataExtracter_RemainHandle::~DataExtracter_RemainHandle()
{
    clear();
}

void DataExtracter_RemainHandle::init(const QVariant& param)
{
    channelNum = param.toInt();
    remainData.swap(QByteArray());
    controlDataLen = channelControlDict[channelNum][2];
    dataLen = channelControlDict[channelNum][1];
    packLen = channelControlDict[channelNum][0];
}

bool DataExtracter_RemainHandle::isArriveHeadFlag(uchar* pos)
{
    //不做空指针判断，外部保证指针合法
    for (int begin = 0, end = headFlag.size(); begin != end; ++begin){
        if (*(pos + packLen + begin) != headFlag[begin])
            return true;
    }

    return false;
}

void DataExtracter_RemainHandle::createDataPack(uchar *pos, QVariantList& container)
{
    QList<QVariant> pack;
    QByteArray controlData, realData;

    std::memcpy(controlData.data(), pos, controlDataLen);
    std::memcpy(realData.data(), pos + controlDataLen, dataLen);

    pack.append(controlData);
    pack.append(realData);
    container.append(pack);
}

void DataExtracter_RemainHandle::handle(QVariant& data)
{
    QVariantList result;
    QByteArray rawData = remainData.append(data.toByteArray());

    int remainDataSize = rawData.size() % packLen;
    std::memcpy(remainData.data(), rawData.data() - remainDataSize, remainDataSize);
    remainData.clear();

    uchar* posPtr = (uchar*)rawData.data();
    for (int begin = 0, end = rawData.size() - packLen - remainDataSize; begin < end; ++begin)
    {
        if (isArriveHeadFlag(posPtr))
        {
            createDataPack(posPtr, result);
            posPtr += packLen;
        }
        else
        {
            posPtr += 1;
        }
    }

    data.swap(QVariant(result));
}

DataSampler_DownSampler::DataSampler_DownSampler(int sampleRate)
    : fetchInterval(-1), buffer()
{
    init(sampleRate);
}

void DataSampler_DownSampler::init(const QVariant &param)
{
    fetchInterval = 100 / param.toInt();
    buffer.swap(QVariant());
}

void DataSampler_DownSampler::handle(QVariant& data)
{
}

void DataFilter_LowpassFilter::handle(QVariant& data)
{

}

void DataFilter_HighpassFilter::handle(QVariant& data){

}

void DataCutter_Noraml::handle(QVariant& data){

}

void CSVWriter::execute(QVariant params){

}

void CSVWriter::run(){

}
