#include "commonvariable.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>

int CommonVariable::openModeBufferMaxSize = 60;

int CommonVariable::dataTransferModeBufferMaxSize = 60;

QString CommonVariable::searchDeviceCommandStr("AT:GS");

QString CommonVariable::connectDeviceCommandStr("AT:GI-#");

QString CommonVariable::newlineStr("\n");

QString CommonVariable::tmpFileDefaultPath("./");

QString CommonVariable::dataFilePattern(".*\.(csv|txt)");

std::map<int, int> CommonVariable::channelNumMap{
    std::pair<int, int>(18, 2),
    std::pair<int, int>(27, 8),
    std::pair<int, int>(51, 16),
    std::pair<int, int>(99, 32)
};

uchar CommonVariable::packHeadFlag1 = 0xaa;
uchar CommonVariable::packHeadFlag2 = 0x55;

//包总字节数  包数据部分字节数  包控制信息字节数  包单个数据的字节数
ChannelControlDict CommonVariable::channelControlDict{
    std::pair<int, std::vector<int>>(2, std::vector<int>{18, 16, 0, 2}),
    std::pair<int, std::vector<int>>(8, std::vector<int>{27, 25, 1, 3}),
    std::pair<int, std::vector<int>>(16, std::vector<int>{51, 49, 1, 3}),
    std::pair<int, std::vector<int>>(32, std::vector<int>{99, 97, 1, 3})
};

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
