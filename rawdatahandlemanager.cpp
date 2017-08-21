#include "rawdatahandlemanager.h"
#include <iterator>

RawDataHandleManager::RawDataHandleManager()
{
}

RawDataHandleManager::~RawDataHandleManager()
{
    for(HandlerList::iterator begin = dataHandlerChain.begin(), end = dataHandlerChain.end(); begin != end; ++begin){
        delete (*begin);
    }

    for(ExecutorMap::iterator begin = intermediateResultHook.begin(), end = intermediateResultHook.end(); begin != end; ++begin){
        delete (*begin);
    }
}

void RawDataHandleManager::handle(QVariant& data)
{
    for(HandlerList::iterator begin = dataHandlerChain.begin(), end = dataHandlerChain.end(); begin != end; ++begin)
    {
        (*begin)->handle(data);
        QPair<ExecutorMap::iterator, ExecutorMap::iterator> hooks = intermediateResultHook.equal_range((*begin)->priority() + (*begin)->identifier());
        while (hooks.first != hooks.second){
            (*hooks.first)->execute(data);
            ++hooks.first;
        }
    }
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
