#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

#include "devicetest.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("DeviceTestManager", DeviceTest::getInstance());
    engine.rootContext()->setContextProperty("RawDataHandleManager", RawDataHandleManager::getInstance());
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}

