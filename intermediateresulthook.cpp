#include "intermediateresulthook.h"

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
}

void CSVWriter::init(const QVariant &param)
{
    QVariantList params = param.toList();

    lock.lock();

    clear();
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
        out << "T" << begin << ',';
    }
    out << endl;
    out.flush();
}

void CSVWriter::clear()
{
    if (file.isOpen()) file.close();
}

void CSVWriter::bind(QThread *thread)
{
    if (!thread) throw std::exception("write thread is invalid");

    writeThread = thread;
    this->moveToThread(writeThread);
    connect(writeThread, &QThread::finished, this, &QObject::deleteLater);
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
           out << dataPack.at(channelCounter).toInt() << ',';
        }
    }
    out << '\n';
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
