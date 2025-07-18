#include <QDebug>
#include <QCoreApplication>
#include <QFileInfo>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>

#include "mymainonbon.h"
#include "myhttpserveronbon.h"
#include "screen.h"

#define CFG_JSON    "/cfg.json"

MyMainOnBon::MyMainOnBon(QObject *parent)
    : QObject{parent}
{
    QJsonObject cfgJson;

    if(loadCfg(cfgJson)){
        initHttpServer(cfgJson);
        initScreen(cfgJson);
        if(m_httpserver && m_screen){
            connect(m_httpserver, &MyHttpServerOnBon::signalTest,
                    m_screen, &Screen::slotTest);
            connect(m_httpserver, &MyHttpServerOnBon::signalDynamicArea_AddAreaTxtDetails_6G_serial,
                    m_screen, &Screen::slotDynamicArea_AddAreaTxtDetails_6G_serial);
            connect(m_httpserver, &MyHttpServerOnBon::signalDynamicArea_DelArea_G5_Serial,
                    m_screen, &Screen::slotDynamicArea_DelArea_G5_Serial);
            connect(m_httpserver, &MyHttpServerOnBon::signalStaticProgram,
                    m_screen, &Screen::slotStaticProgram);
            connect(m_httpserver, &MyHttpServerOnBon::signalSetLuminance,
                    m_screen, &Screen::slotSetLuminance);
        }
    }
}

bool MyMainOnBon::loadCfg(QJsonObject& cfgJson)
{
    // CFG_JSON配置文件是否存在
    QString iniPath = QCoreApplication::applicationDirPath() + CFG_JSON;

    if(!QFileInfo::exists(iniPath)){
        qCritical() << QString("****** lost %1 ******").arg(CFG_JSON);
        return false;
    }

    // 打开配置文件
    QFile file(iniPath);
    if(!file.open(QIODevice::ReadWrite)){
        qCritical() << QString("****** %1 open failed ******").arg(iniPath);
        return false;
    }

    // 读取配置文件
    QJsonParseError error;
    cfgJson = QJsonDocument::fromJson(file.readAll(), &error)
                  .object();

    if(error.error != QJsonParseError::NoError){
        qCritical() << "error cfgJson: " + error.errorString();
        return false;
    }

    return true;
}

void MyMainOnBon::initHttpServer(QJsonObject &cfgJson)
{
    int port = cfgJson.value("httpServer").toObject().value("port").toInt();
    if(port < 1023 || port > 65535){
        qCritical() << QString("****** error port: %1 ******").arg(port);
        return;
    }

    m_httpserver = new MyHttpServerOnBon(port, cfgJson, this);
}

void MyMainOnBon::initScreen(QJsonObject &cfgJson)
{
    m_screen = new Screen(cfgJson.value("screen").toObject().value("portName").toString(), this);
}
