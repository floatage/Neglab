#include "datahandler.h"
#include <cstring>
#include <QVariantList>
#include <QDebug>

DataExtracter_RemainHandle::DataExtracter_RemainHandle()
{
}

DataExtracter_RemainHandle::DataExtracter_RemainHandle(int iChannelNum)
{
    init(iChannelNum);
}

DataExtracter_RemainHandle::~DataExtracter_RemainHandle()
{
}

void DataExtracter_RemainHandle::init(const QVariant& param)
{
    channelNum = param.toInt();
    if (channelNum <= 0 || CommonVariable::channelControlDict.count(channelNum)==0) throw std::exception("invalid channel number");

    remainData.swap(QByteArray());
    channelDataLen = CommonVariable::channelControlDict[channelNum][3];
    controlDataLen = CommonVariable::channelControlDict[channelNum][2];
    dataLen = CommonVariable::channelControlDict[channelNum][1];
    packLen = CommonVariable::channelControlDict[channelNum][0];
}

//bool DataExtracter_RemainHandle::isArriveHeadFlag(uchar* pos)
//{
//    //不做空指针判断，外部保证指针合法
//    printf("0x%x 0x%x\n",*(pos+packLen), *(pos+packLen+1));
//    for (int begin = 0, end = headFlag.size(); begin != end; ++begin){
//        if (*(pos + packLen + begin) != headFlag[begin])
//            return false;
//    }

//    return true;
//}

int DataExtracter_RemainHandle::byteToInt(uchar *head, int len)
{
    int result = 0;
    for (int begin = 0; begin < len; ++begin){
        result = (result << 8) & *(head+begin);
    }
    return result;
}

void DataExtracter_RemainHandle::createDataPack(uchar *pos, QVariantList& container)
{
    QByteArray controlData(controlDataLen, CommonVariable::channelDefaultControlInfor);
    std::memcpy(controlData.data(), pos, controlDataLen);
    pos += controlDataLen;

    for (int begin = 0, end = dataLen / channelNum; begin != end; ++begin)
    {
        QVariantList dataPack;
        for (int channelCounter = 0; channelCounter < channelNum; ++channelCounter)
        {
            dataPack.append(byteToInt(pos, channelDataLen));
            pos += channelDataLen;
        }

        dataPack.push_front(controlData.toInt());
        container.append(QVariant(dataPack));
    }
}

void DataExtracter_RemainHandle::handle(QVariant& data)
{
    if (data.isNull()) return;

    bool isFirst = true;
    QVariantList result;
    QByteArray rawData = remainData.append(data.toByteArray());

    int remainDataSize = 0;
    uchar* posPtr = (uchar*)rawData.data();
    for (int begin = 0, end = rawData.size() - packLen - remainDataSize; begin < end; ++begin)
    {
        if (*(posPtr+packLen) == CommonVariable::packHeadFlag1 && *(posPtr+packLen+1) == CommonVariable::packHeadFlag2)
        {
            if (isFirst){
                int remainDataSize = (rawData.size()-begin) % packLen;
                remainData.swap(QByteArray());
                std::memcpy(remainData.data(), rawData.data() + rawData.size() - remainDataSize, remainDataSize);
                isFirst = false;
                end = rawData.size() - packLen - remainDataSize;
            }

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

DataSampler_DownSampler::DataSampler_DownSampler()
{
}

DataSampler_DownSampler::DataSampler_DownSampler(int sampleRate)
    : fetchInterval(-1), validDataList()
{
    init(sampleRate);
}

void DataSampler_DownSampler::init(const QVariant &param)
{   
    fetchInterval = 100 / param.toInt();
    validDataList.swap(QVariantList());

    if (fetchInterval < 0 || fetchInterval > 100) throw std::exception("invalid sample");
}

void DataSampler_DownSampler::handle(QVariant& data)
{
    if (data.isNull() || fetchInterval == 1) return;

    validDataList.push_back(data.toList());
    if (validDataList.size() < fetchInterval)
    {
        data.swap(QVariant());
        return ;
    }

    QVariantList result;
    int posSelect = fetchInterval, validDataLen = validDataList.size();
    while (posSelect < validDataLen){
        result.append(validDataList.first());
        validDataList.erase(validDataList.begin(), validDataList.begin() + fetchInterval);
        posSelect += fetchInterval;
    }
    result.append(validDataList.first());
    validDataList.erase(validDataList.begin());

    data.swap(QVariant(result[0]));
}

DataFilter_IIR::DataFilter_IIR()
{
}

DataFilter_IIR::DataFilter_IIR(int iBufferLen, int iChannelNum, const QVector<float> &ia, const QVector<float> &ib)
    :bufferLen(iBufferLen), channelNum(iChannelNum), a(ia), b(ib), channelHistoryList(), dataMapFuncPtr(NULL), bufferWritePos(0)
{
    if (iBufferLen <= 0 || iChannelNum <= 0 || ia.size() < iBufferLen || ib.size() < iBufferLen) throw std::exception("invalid params");

    for (int begin = 0; begin < channelNum; ++begin){
        QVector<float>* u = new QVector<float>(bufferLen, 0.0f);
        channelHistoryList.append(u);
    }
    dataMapFuncPtr = CommonVariable::getMatchDataMapFuncPtr(channelNum);
}

DataFilter_IIR::~DataFilter_IIR()
{
    clear();
}

void DataFilter_IIR::clear()
{
    a.clear();
    b.clear();
    bufferWritePos = 0;
    for (int begin = 0; begin < channelNum; ++begin){
        delete channelHistoryList[begin];
    }
}

//此处只做运行时的重新初始化，不做构造函数的初始化, 因为出现了多线程堆越界问题，只有此方法可解决
void DataFilter_IIR::init(const QVariant &param)
{
    clear();

    QVariantList params = param.toList();
    bufferLen = params[0].toInt();
    channelNum = params[1].toInt();

    QVariantList abValue = params[2].toList();
    for (QVariantList::iterator it = abValue.begin(); it != abValue.end(); ++it)
        a.append(it->toFloat());

    abValue = params[3].toList();
    for (QVariantList::iterator it = abValue.begin(); it != abValue.end(); ++it)
        b.append(it->toFloat());

    if (bufferLen <= 0 || channelNum <= 0 || a.size() < bufferLen || b.size() < bufferLen) throw std::exception("invalid params");

    for (int begin = 0; begin < channelNum; ++begin){
        QVector<float>* u = new QVector<float>(bufferLen, 0.0f);
        channelHistoryList.append(u);
    }
    dataMapFuncPtr = CommonVariable::getMatchDataMapFuncPtr(channelNum);
}

void DataFilter_IIR::handle(QVariant& data)
{
    if (data.isNull()) return ;

    QVariantList midData = data.toList();
    for (int packCounter = 0, packSize = midData.size(); packCounter < packSize; ++packCounter)
    {
        QVariantList packData = midData.at(packCounter).toList();

        //通道0表示控制信号，不做滤波
        for (int channelCounter = 1, validChannelNum = packData.size(); channelCounter < validChannelNum; ++channelCounter)
        {
            float value = (*dataMapFuncPtr)(packData.at(channelCounter).toInt(), 1.0);

            for (int begin = 1, end = a.size(); begin < end; ++begin){
                value -= a[begin] * channelHistoryList[channelCounter-1]->at((bufferWritePos + begin) %  bufferLen);
            }

            channelHistoryList[channelCounter-1]->replace(bufferWritePos, value);

            value = 0.0;
            for (int begin = 0, end = b.size(); begin < end; ++begin){
                value += b[begin] * channelHistoryList[channelCounter-1]->at((bufferWritePos + begin) %  bufferLen);
            }

            packData.replace(channelCounter, value);
        }

        midData.replace(packCounter, packData);
        bufferWritePos = (bufferWritePos + bufferLen - 1) % bufferLen;
    }

    data.swap(QVariant(midData));
}
