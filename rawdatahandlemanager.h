#ifndef RAWDATAHANDLEMANAGER_H
#define RAWDATAHANDLEMANAGER_H

#include <QVariant>
#include <QList>
#include <QMultiMap>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QObject>

class DataHandler
{
public:
    virtual void init(const QVariant& param) = 0;
    virtual void handle(QVariant& data) = 0;
    virtual void clear() = 0;
    virtual int priority() = 0;
    virtual int identifier() = 0;
    virtual ~DataHandler(){}
};

class DataExtracter: public DataHandler
{
public:
    void init(const QVariant&){}
    void handle(QVariant& data) = 0;
    void clear(){}
    int priority(){ return 1000;}
    int identifier() = 0;
    ~DataExtracter(){}
};

class DataSampler: public DataHandler
{
public:
    void init(const QVariant&){}
    void handle(QVariant& data) = 0;
    void clear(){}
    int priority(){ return 4000;}
    int identifier() = 0;
    ~DataSampler(){}
};

class DataResolver: public DataHandler
{
public:
    void init(const QVariant&){}
    void handle(QVariant& data) = 0;
    void clear(){}
    int priority(){ return 7000;}
    int identifier() = 0;
    ~DataResolver(){}
};

class DataFilter: public DataHandler
{
public:
    void init(const QVariant&){}
    void handle(QVariant& data) = 0;
    void clear(){}
    int priority(){ return 10000;}
    int identifier() = 0;
    ~DataFilter(){}
};

class DataCutter: public DataHandler
{
public:
    void init(const QVariant&){}
    void handle(QVariant& data) = 0;
    void clear(){}
    int priority(){ return 13000;}
    int identifier() = 0;
    ~DataCutter(){}
};

class ExecuteObject: public QThread
{
public:
    virtual void init(const QVariant&) = 0;
    virtual void clear() = 0;
    virtual void execute(const QVariant& params) = 0;
    virtual int identifier() = 0;
    virtual ~ExecuteObject(){}
};

class RawDataHandleManager: public QThread
{
    Q_OBJECT
public:
    ~RawDataHandleManager();

    static RawDataHandleManager* getInstance();

    void run();
    void stop();
    void init();
    void clear();
    void handle(const QByteArray& data);

    bool addHandler(DataHandler* handler);
    bool deleteHandler(int priority, int identifier);

    bool addIntermediateResultHook(int priority, int identifier, ExecuteObject* executor);
    bool deleteIntermediateResultHook(int priority, int identifier, int hookIdentifier);

private:
    typedef QList<DataHandler*> HandlerList;
    typedef QMultiMap<int, ExecuteObject*> ExecutorMap;

    RawDataHandleManager();
    RawDataHandleManager(const RawDataHandleManager&);
    RawDataHandleManager& operator =(const RawDataHandleManager&);

    static RawDataHandleManager* instance;

    bool canRun;
    QMutex lock;
    QVariant buffer;
    QWaitCondition signal;
    HandlerList dataHandlerChain;
    ExecutorMap intermediateResultHook;

signals:
    void dataHandleFinished(QVariant plotData);
};

#endif // RAWDATAHANDLEMANAGER_H
