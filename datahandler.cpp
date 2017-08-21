#include "datahandler.h"
#include <cstring>
#include <QList>
#include <QVariantList>
#include <QTextStream>

DataExtracter_RemainHandle::DataExtracter_RemainHandle(int iChannelNum)
    : headFlag{0xaa, 0x55}, channelControlDict()
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
    remainData.swap(QByteArray());
    channelDataLen = channelControlDict[channelNum][3];
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
    QByteArray controlData(0x00);
    std::memcpy(controlData.data(), pos, controlDataLen);
    pos += controlDataLen;

    for (int begin = 0, end = dataLen / channelNum; begin != end; ++begin){
        QList<QVariant> pack;
        for (int channelCounter = 0; channelCounter < channelNum; ++channelCounter){
            pack.append(byteToInt(pos, channelDataLen));
            pos += channelDataLen;
        }

        pack.push_front(controlData.toInt());
        container.append(pack);
    }
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
    : fetchInterval(-1), remainData()
{
    init(sampleRate);
}

void DataSampler_DownSampler::init(const QVariant &param)
{
    fetchInterval = 100 / param.toInt();
    remainData.swap(QVariant());
}

void DataSampler_DownSampler::handle(QVariant& data)
{
    QVariantList *newDataPtr = NULL, *remainDataPtr = NULL, result;
    data.convert(QVariant::List, newDataPtr);
    remainData.convert(QVariant::List, remainDataPtr);
    if (newDataPtr->size() < fetchInterval){
        remainData.swap(data);
        return ;
    }

    int posSelect = 0, remainSize = remainDataPtr->size();
    while (posSelect < remainSize){
        result.append(remainDataPtr->at(posSelect));
        posSelect += fetchInterval;
    }

    while (posSelect - remainSize <= newDataPtr->size()) {
        result.append(newDataPtr->at(posSelect - remainSize));
        posSelect += fetchInterval;
    }

    remainData.swap(QVariant());
    data.swap(QVariant(result));
}

void DataFilter_LowpassFilter::handle(QVariant& data)
{

}

void DataFilter_HighpassFilter::handle(QVariant& data){

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
    signal.lock();
    isRuning = true;
    this->start();
}

void CSVWriter::clear()
{
    stop();
    if (file.isOpen()) file.close();
}

void CSVWriter::execute(const QVariant& params)
{
    buffer.setValue(params);
    signal.unlock();
}

void CSVWriter::run(){
    QTextStream out(&file);

    out << "C,";
    for (int begin = 0; begin < channelNum; ++begin){
        out << "T" << begin << ',';
    }
    out << endl;

    while(isRuning){
        signal.lock();
        QVariantList* dataList = NULL;
        buffer.convert(QVariant::List, dataList);
        for (int begin = 0; begin < dataList->size(); ++begin){
            QVariantList* dataPack = NULL;
            dataList->at(begin).convert(QVariant::List, dataPack);
            for (int channelCounter = 0, validChannleNum = dataPack->size(); channelCounter < validChannleNum; ++channelCounter){
               out << dataPack->at(channelCounter).toInt() << ',';
            }
        }
        out << '\n';
        out.flush();
        signal.lock();
    }
}

void CSVWriter::stop()
{
    isRuning = false;
    signal.unlock();
}
