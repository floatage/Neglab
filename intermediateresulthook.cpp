#include "intermediateresulthook.h"

#include <QTextStream>

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
    dataQueue.clear();
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


