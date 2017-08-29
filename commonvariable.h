#ifndef COMMONVARIABLE_H
#define COMMONVARIABLE_H

#include <QString>
#include <map>
#include <vector>

typedef float (*DataMapFuncPtr)(int, float);
typedef std::map<int, std::vector<int>> ChannelControlDict;

class CommonVariable
{
public:
    static int openModeBufferMaxSize;

    static int dataTransferModeBufferMaxSize;

    static int dataHandleLoopReactiveTimes;

    static QString searchDeviceCommandStr;

    static QString connectDeviceCommandStr;

    static QString newlineStr;

    static QString tmpFileDefaultPath;

    static QString dataFilePattern;

    static std::map<int, int> channelNumMap;

    static uchar packHeadFlag1, packHeadFlag2;

    static ChannelControlDict channelControlDict;

    static uchar channelDefaultControlInfor;

    static int historyDataBufferLen;

    static DataMapFuncPtr getMatchDataMapFuncPtr(int channelNum);

    static bool copyCurDirFile(const QString &srcFilePath, const QString &tgtFilePath, const QString &pattern);

    //设置可改成从配置文件中读取
    static void load(const QString& settingPath);

    static void save(const QString& settingPath);

private:
    inline static float dataMap_channel2(int x, float);

    inline static float dataMap_channel8(int x, float gain);

    inline static float dataMap_channel16(int x, float gain);

    inline static float dataMap_channel32(int x, float gain);
};

#endif // COMMONVARIABLE_H
