#include "rawdatahandlemanager.h"
#include <iterator>

RawDataHandleManager* RawDataHandleManager::instance = NULL;
RawDataHandleManager::RawDataHandleManager()
    :dataHandlerChain(), intermediateResultHook(), lock(), signal(), buffer()
{
    init();
}

RawDataHandleManager::~RawDataHandleManager()
{
    clear();
}

void RawDataHandleManager::init()
{
    clear();
    canRun = true;
}

void RawDataHandleManager::clear()
{
    stop();
    for(HandlerList::iterator begin = dataHandlerChain.begin(), end = dataHandlerChain.end(); begin != end; ++begin){
        delete (*begin);
    }
    dataHandlerChain.swap(HandlerList());

    for(ExecutorMap::iterator begin = intermediateResultHook.begin(), end = intermediateResultHook.end(); begin != end; ++begin){
        delete (*begin);
    }
    intermediateResultHook.swap(ExecutorMap());
    buffer.swap(QVariant());
}

RawDataHandleManager* RawDataHandleManager::getInstance()
{
    //这里可能有线程安全问题
    if (instance == NULL){
        instance = new RawDataHandleManager();
    }
    return instance;
}

void RawDataHandleManager::handle(const QByteArray& data)
{
    if (!this->isRunning()){
        this->start();
        this->sleep(1);
    }

    lock.lock();
    buffer.swap(QVariant(data));
    signal.wakeOne();
    lock.unlock();
}

void RawDataHandleManager::run()
{
    while(true)
    {
        lock.lock();
        signal.wait(&lock);

        if (!canRun) {
            lock.unlock();
            break;
        }

        for(HandlerList::iterator begin = dataHandlerChain.begin(), end = dataHandlerChain.end(); begin != end; ++begin)
        {
            (*begin)->handle(buffer);
            QPair<ExecutorMap::iterator, ExecutorMap::iterator> hooks = intermediateResultHook.equal_range((*begin)->priority() + (*begin)->identifier());
            while (hooks.first != hooks.second){
                (*hooks.first)->execute(buffer);
                ++hooks.first;
            }
        }

        emit dataHandleFinished(buffer);
        lock.unlock();
    }
}

void RawDataHandleManager::stop()
{
    lock.lock();
    canRun = false;
    signal.wakeOne();
    lock.unlock();
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
            intermediateResultHook.erase(range.first);
            delete (*range.first);
            return true;
        }

        ++range.first;
    }

    return false;
}
