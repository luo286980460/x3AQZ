#include <QDebug>
#include <QCoreApplication>
#include <QFileInfo>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>

#include "mymainkafka.h"
#include "myhttpserverkafka.h"

#define CFG_JSON    "/cfg.json"

MyMainKafka::MyMainKafka(QObject *parent)
    : QObject{parent}
{
    QJsonObject cfgJson;

    if(loadCfg(cfgJson)){
        initHttpServer(cfgJson);
    }
}

bool MyMainKafka::loadCfg(QJsonObject& cfgJson)
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

void MyMainKafka::initHttpServer(QJsonObject &cfgJson)
{
    int port = cfgJson.value("httpServer").toObject().value("port").toInt();
    if(port < 1023 || port > 65535){
        qCritical() << QString("****** error port: %1 ******").arg(port);
        return;
    }

    m_httpserver = new MyHttpServerKafka(port, cfgJson, this);
}
