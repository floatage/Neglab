#include "commonvariable.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>

//非数据采集模式时设备缓冲区大小(字符)，超出此大小则处理数据并清空
int CommonVariable::openModeBufferMaxSize = 60;

//数据采集模式时设备缓冲区大小(字节)，超出则处理
int CommonVariable::dataTransferModeBufferMaxSize = 60;

//为防止数据处理循环信号丢失，采集到一定次数的数据则手动激活循环
int CommonVariable::dataHandleLoopReactiveTimes = 20;

//向串口发送的搜索采集设备的指令
QString CommonVariable::searchDeviceCommandStr("AT:GS");

//向串口发送的连接指定编号采集设备的指令
QString CommonVariable::connectDeviceCommandStr("AT:GI-#");

//文件中换行符
QString CommonVariable::newlineStr("\n");

//临时文件的存储路径，若路径不存在不会创建，即会写入失败
QString CommonVariable::tmpFileDefaultPath("./");

//系统认为是数据文件的文件名的规则
QString CommonVariable::dataFilePattern(".*\.(csv|txt)");

//数据包大小和通道设备数的映射关系，主要用作判断设备通道数
std::map<int, int> CommonVariable::channelNumMap{
    std::pair<int, int>(18, 2),
    std::pair<int, int>(27, 8),
    std::pair<int, int>(51, 16),
    std::pair<int, int>(99, 32)
};

//包标识，只能为两个字符，若大于则需修改判断设备通道函数和数据分包处理组件处的代码
uchar CommonVariable::packHeadFlag1 = 0xaa;
uchar CommonVariable::packHeadFlag2 = 0x55;

//包总字节数  包数据部分字节数  包控制信息字节数  包单个数据的字节数， 主要用作如何对数据进行分包
ChannelControlDict CommonVariable::channelControlDict{
    std::pair<int, std::vector<int>>(2, std::vector<int>{18, 16, 0, 2}),
    std::pair<int, std::vector<int>>(8, std::vector<int>{27, 25, 1, 3}),
    std::pair<int, std::vector<int>>(16, std::vector<int>{51, 49, 1, 3}),
    std::pair<int, std::vector<int>>(32, std::vector<int>{99, 97, 1, 3})
};

//默认通道的mark信息,分包时对于无控制信息的会在包头添加默认的mark
uchar CommonVariable::channelDefaultControlInfor = 0x00;

int CommonVariable::historyDataBufferLen = 10;

//内联函数通常定义在头文件中，因其为编译器期行为。并且通过函数指针调用时会生成函数体
//因若无函数体则无法使用函数指针调用函数，并且调试器可能无法调试内联函数，内联函数升级时必须编译
//则下面函数定义实际得不到想要的效果
float CommonVariable::dataMap_channel2(int x, float){
   // (x - 2^16) / 2^16
   return  (x - 65536.0f) / 65536.0f;
}

float CommonVariable::dataMap_channel8(int x, float gain){
    // (x - 2^16) / 2^16 * 2.0*10^6
    return (x - 65536.0f) / 65536.0f * 2000000.0f / gain;
}

float CommonVariable::dataMap_channel16(int x, float gain){
    // (x - 2^24) / 2^24 * 2.0*10^6
    return (x - 16777216.0f) / 16777216.0f * 2000000.0f / gain;
}

float CommonVariable::dataMap_channel32(int x, float gain){
    // (x - 2^24) / 2^24 * 2.0*10^6
    return (x - 16777216.0f) / 16777216.0f * 2000000.0f / gain;
}

DataMapFuncPtr CommonVariable::getMatchDataMapFuncPtr(int channelNum)
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

//windows下的正确的路径应该为E:\\...
bool CommonVariable::copyCurDirFile(const QString &srcFilePath, const QString &tgtFilePath, const QString& pattern)
{
    QFileInfo srcFileInfo(srcFilePath);
    if (srcFileInfo.isDir()) {
        QDir targetDir(tgtFilePath);
        QDir sourceDir(srcFilePath);
        QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::NoDotAndDotDot);
        QRegExp fileFilter(pattern);

        foreach (QString fileName, fileNames)
        {
            if (!fileFilter.exactMatch(fileName))
                continue;

            QString path = sourceDir.absolutePath() + "\\" + fileName;
            QString newPath = targetDir.absolutePath() + "\\" + fileName;
            path.replace("/", "\\");
            newPath.replace("/", "\\");

            if (targetDir.exists(newPath)){
                targetDir.remove(newPath);
            }

            if (!QFile::copy(path, newPath)){
                return false;
            }
        }

        return true;
    }

    return false;
}

void CommonVariable::load(const QString &settingPath)
{
}

void CommonVariable::save(const QString &settingPath)
{
}
