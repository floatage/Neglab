#include "intermediateresulthook.h"
#include "commonvariable.h"

CSVWriter::CSVWriter()
{
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
    if (writeThread) writeThread->deleteLater();
}

void CSVWriter::init(const QVariant &param)
{
    QVariantList params = param.toList();

    clear();

    lock.lock();
    fileName = params[0].toString();
    file.setFileName(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    out.setDevice(&file);

    dataQueue.clear();
    channelNum = params[1].toInt();
    isLoop = false;

    lock.unlock();

    out << "C,";
    for (int begin = 0; begin < channelNum; ++begin){
        out << "T" << begin;
        if (begin != channelNum-1){
            out << ',';
        }
    }
    out << CommonVariable::newlineStr;
    out.flush();
}

void CSVWriter::clear()
{
    lock.lock();
    if (file.isOpen()) file.close();
    lock.unlock();
}

void CSVWriter::bind(QThread *thread)
{
    if (!thread) throw std::exception("write thread is invalid");

    writeThread = thread;
    this->moveToThread(writeThread);
    connect(this, &CSVWriter::hasDataCanWrite, this, &CSVWriter::writeData);
    writeThread->start();
}

void CSVWriter::execute(const QVariant& params)
{
    dataQueue.push_back(params);

    if (!isLoop){
        lock.lock();
        isLoop = true;
        lock.unlock();
        emit hasDataCanWrite();
    }
}

void CSVWriter::writeData()
{
    QVariantList dataList = dataQueue.first().toList();
    for (int begin = 0; begin < dataList.size(); ++begin)
    {
        QVariantList dataPack = dataList.at(begin).toList();
        for (int channelCounter = 0, validChannleNum = dataPack.size(); channelCounter < validChannleNum; ++channelCounter)
        {
           out << dataPack.at(channelCounter).toFloat();
           if (channelCounter != validChannleNum-1){
               out << ',';
           }
        }
        out << CommonVariable::newlineStr;
    }

    out.flush();
    dataQueue.pop_front();
    if (!dataQueue.isEmpty())
    {
        emit hasDataCanWrite();
    }
    else
    {
        lock.lock();
        isLoop = false;
        lock.unlock();
    }
}
