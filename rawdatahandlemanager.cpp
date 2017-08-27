#include "rawdatahandlemanager.h"
#include <iterator>
#include <QDebug>

RawDataHandleManager* RawDataHandleManager::instance = NULL;
RawDataHandleManager::RawDataHandleManager()
    :dataHandlerChain(), intermediateResultHook()
{
    init();
}

RawDataHandleManager::~RawDataHandleManager()
{
    clear();
    if (RawDataHandleManager::instance != NULL){
        delete RawDataHandleManager::instance;
    }
}

void RawDataHandleManager::init()
{
    clear();
}

void RawDataHandleManager::clear()
{
    for(HandlerList::iterator begin = dataHandlerChain.begin(), end = dataHandlerChain.end(); begin != end; ++begin){
        delete (*begin);
        *begin = NULL;
    }
    dataHandlerChain.swap(HandlerList());

    for(ExecutorMap::iterator begin = intermediateResultHook.begin(), end = intermediateResultHook.end(); begin != end; ++begin){
        delete (*begin);
        *begin = NULL;
    }
    intermediateResultHook.swap(ExecutorMap());
}

RawDataHandleManager* RawDataHandleManager::getInstance()
{
    //这里可能有线程安全问题
    if (instance == NULL){
        instance = new RawDataHandleManager();
    }
    return instance;
}

void RawDataHandleManager::handleDeviceByteBufferFilled(QVariant buffer)
{
    for(HandlerList::iterator begin = dataHandlerChain.begin(), end = dataHandlerChain.end(); begin != end; ++begin)
    {
        (*begin)->handle(buffer);
        QPair<ExecutorMap::iterator, ExecutorMap::iterator> hooks = intermediateResultHook.equal_range((*begin)->priority() + (*begin)->identifier());
        while (hooks.first != hooks.second){
            (*hooks.first)->execute(buffer);
            ++hooks.first;
        }
        qDebug() << QThread::currentThreadId() << "  "<<  (*begin)->priority()+(*begin)->identifier() << "handler handle finished;" << endl;
    }

    qDebug() << QThread::currentThreadId() << "handle finished;Next" << endl;
    emit getNextBuffer(buffer);
//    emit dataHandleFinished(buffer);
}

bool RawDataHandleManager::addHandler(DataHandler* handler)
{
    if (handler == NULL) return false;

    int priority = handler->priority(), identifier = handler->identifier();
    for (HandlerList::iterator begin = dataHandlerChain.begin(), end = dataHandlerChain.end(); begin != end; ++begin)
    {
        if (priority <= (*begin)->priority() && identifier <= (*begin)->identifier()){
            dataHandlerChain.insert(begin, handler);
            return true;
        }
    }

    dataHandlerChain.append(handler);
    return true;
}

bool RawDataHandleManager::deleteHandler(int priority, int identifier)
{
    for (HandlerList::iterator begin = dataHandlerChain.begin(), end = dataHandlerChain.end(); begin != end; ++begin)
    {
        if (priority == (*begin)->priority() && identifier == (*begin)->identifier()){
            delete (*begin);
            dataHandlerChain.erase(begin);
            return true;
        }
    }
    return false;
}

bool RawDataHandleManager::addIntermediateResultHook(int priority, int identifier, ExecuteObject* executor)
{
    if (executor == NULL) return false;

    int handlerId = priority + identifier;
    intermediateResultHook.insert(handlerId, executor);
    executor->start();
    return true;
}

bool RawDataHandleManager::deleteIntermediateResultHook(int priority, int identifier, int hookIdentifier)
{
    int handlerId = priority + identifier;
    QPair<ExecutorMap::iterator, ExecutorMap::iterator> range = intermediateResultHook.equal_range(handlerId);

    while (range.first != range.second){
        if ((*range.first)->identifier() == hookIdentifier){
            delete (*range.first);
            intermediateResultHook.erase(range.first);
            return true;
        }

        ++range.first;
    }

    return false;
}
