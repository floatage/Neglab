#include "datahandler.h"
#include <cstring>
#include <QVariantList>
#include <QVariantMap>
#include <QTextStream>
#include <QDebug>

DataExtracter_RemainHandle::DataExtracter_RemainHandle(int iChannelNum)
    :channelControlDict()
{
    channelControlDict[2] = {18, 16, 0, 2};
    channelControlDict[8] = {27, 25, 1, 3};
    channelControlDict[16] = {51, 49, 1, 3};
    channelControlDict[32] = {99, 97, 1, 3};
    init(iChannelNum);
}

DataExtracter_RemainHandle::~DataExtracter_RemainHandle()
{
}

void DataExtracter_RemainHandle::init(const QVariant& param)
{
    channelNum = param.toInt();
    if (channelNum <= 0) throw std::exception("invalid channel number");

    remainData.swap(QByteArray());
    channelDataLen = channelControlDict[channelNum][3];
    controlDataLen = channelControlDict[channelNum][2];
    dataLen = channelControlDict[channelNum][1];
    packLen = channelControlDict[channelNum][0];
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
    QByteArray controlData(controlDataLen, 0x00);
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
    static const uchar packHeadFlag1 = 0xaa, packHeadFlag2 = 0x55;

    if (data.isNull()) return;

    bool isFirst = true;
    QVariantList result;
    QByteArray rawData = remainData.append(data.toByteArray());

    int remainDataSize = 0;
    uchar* posPtr = (uchar*)rawData.data();
    for (int begin = 0, end = rawData.size() - packLen - remainDataSize; begin < end; ++begin)
    {
        if (*(posPtr+packLen) == packHeadFlag1 && *(posPtr+packLen+1) == packHeadFlag2)
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
    if (data.isNull() || fetchInterval == 2) return;

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

//内联函数通常定义在头文件中，因其为编译器期行为。并且通过函数指针调用时会生成函数体
//因若无函数体则无法使用函数指针调用函数，并且调试器可能无法调试内联函数，内联函数升级时必须编译
//则下面函数定义实际得不到想要的效果
//inline float dataMap_channel2(int x, float){
//   // (x - 2^16) / 2^16
//   return  (x - 65536.0f) / 65536.0f;
//}

float dataMap_channel2(int x, float){
   // (x - 2^16) / 2^16
   return  (x - 65536.0f) / 65536.0f;
}

float dataMap_channel8(int x, float gain){
    // (x - 2^16) / 2^16 * 2.0*10^6
    return (x - 65536.0f) / 65536.0f * 2000000.0f / gain;
}

float dataMap_channel16(int x, float gain){
    // (x - 2^24) / 2^24 * 2.0*10^6
    return (x - 16777216.0f) / 16777216.0f * 2000000.0f / gain;
}

float dataMap_channel32(int x, float gain){
    // (x - 2^24) / 2^24 * 2.0*10^6
    return (x - 16777216.0f) / 16777216.0f * 2000000.0f / gain;
}

DataFilter_IIR::DataFilter_IIR(int iBufferLen, int iChannelNum, const QVector<float> &ia, const QVector<float> &ib)
    :bufferLen(iBufferLen), channelNum(iChannelNum), a(ia), b(ib), channelHistoryList(), dataMapFuncPtr(NULL), bufferWritePos(0)
{
    if (iBufferLen <= 0 || iChannelNum <= 0 || ia.size() < iBufferLen || ib.size() < iBufferLen) throw std::exception("invalid params");

    for (int begin = 0; begin < channelNum; ++begin){
        QVector<float>* u = new QVector<float>(bufferLen, 0.0f);
        channelHistoryList.append(u);
    }
    dataMapFuncPtr = getMatchDataMapFuncPtr(channelNum);
}

DataFilter_IIR::~DataFilter_IIR()
{
    for (int begin = 0; begin < channelNum; ++begin){
        delete channelHistoryList[begin];
    }
}

DataFilter_IIR::DataMapFuncPtr DataFilter_IIR::getMatchDataMapFuncPtr(int channelNum)
{
    switch (channelNum) {
        case 2:
            return &dataMap_channel2;
        case 8:
            return &dataMap_channel8;
        case 16:
            return &dataMap_channel16;
        case 32:
            return &dataMap_channel32;
        default:
            return NULL;
    }
}

void DataFilter_IIR::handle(QVariant& data)
{
    if (data.isNull()) return ;

    QVariantMap result;
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

        result.insert(QString::number(packCounter), packData);
        bufferWritePos = (bufferWritePos + bufferLen - 1) % bufferLen;
    }

    data.swap(QVariant(result));
}

CSVWriter::CSVWriter(const QString &iFilename, int iChannelNum)
{
    QVariantList params;
    params.append(iFilename);
    params.append(iChannelNum);
    init(params);
}

CSVWriter::~CSVWriter()
{
    clear();
}

void CSVWriter::init(const QVariant &param)
{
    QVariantList params = param.toList();

    clear();
    fileName = params[0].toString();
    file.setFileName(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    buffer.swap(QVariant());
    channelNum = params[1].toInt();
    canRun = true;
}

void CSVWriter::clear()
{
    stop();
    if (file.isOpen()) file.close();
}

void CSVWriter::execute(const QVariant& params)
{
    lock.lock();
    buffer.setValue(params);
    signal.wakeOne();
    lock.unlock();
}

void CSVWriter::run(){
    QTextStream out(&file);

    out << "C,";
    for (int begin = 0; begin < channelNum; ++begin){
        out << "T" << begin << ',';
    }
    out << endl;

    while(true)
    {
        lock.lock();
        signal.wait(&lock);

        if (!canRun){
            lock.unlock();
            break;
        }

        QVariantList* dataList = NULL;
        buffer.convert(QVariant::List, dataList);
        for (int begin = 0; begin < dataList->size(); ++begin)
        {
            QVariantList* dataPack = NULL;
            dataList->at(begin).convert(QVariant::List, dataPack);
            for (int channelCounter = 0, validChannleNum = dataPack->size(); channelCounter < validChannleNum; ++channelCounter)
            {
               out << dataPack->at(channelCounter).toInt() << ',';
            }
        }
        out << '\n';
        out.flush();

        lock.unlock();
    }
}

void CSVWriter::stop()
{
    lock.lock();
    canRun = false;
    signal.wakeOne();
    lock.unlock();
}
