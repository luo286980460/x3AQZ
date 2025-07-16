#include <QDebug>
#include <QCoreApplication>
#include <QFileInfo>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDateTime>
#include <QJsonArray>

#include "mymain.h"
#include "detectionmode.h"
#include "gps.h"
#include "i2cmanager.h"
#include "myhttpserver.h"
#include "radar.h"

#define CFG_JSON    "/cfg.json"

MyMain::MyMain(QObject *parent)
    : QObject{parent}
{
    init();
    initTimer();
}

// void MyMain::initTimer() /
// {
//     m_timer = new QTimer;
//     connect(m_timer, &QTimer::timeout, this, [this]{
//         m_data2BackServerHeartbeat["controlState"] = false; // 管控状态
//         m_data2BackServerHeartbeat["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

//         QJsonObject json;
//         json.insert("topic", m_heartBeatTopic);
//         json.insert("key", "heartBeat");
//         json.insert("data", m_data2BackServerHeartbeat);
//         sendPostRequestKafka(json);

//         // qDebug() << "post:" << json;
//     });
//     m_timer->setInterval(m_heartBeatInterval);

//     m_timer->start();
// }

void MyMain::initTimer()
{
    m_timer = new QTimer;
    connect(m_timer, &QTimer::timeout, this, [this]{
        // 心跳
        m_heartBeatCount++;
        if(m_heartBeatInterval == m_heartBeatCount){
            m_heartBeatCount = 0;

            m_data2BackServerHeartbeat["controlState"] = m_controlState; // 管控状态
            m_data2BackServerHeartbeat["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

            QJsonObject json;
            json.insert("topic", m_heartBeatTopic);
            json.insert("key", "heartBeat");
            json.insert("data", m_data2BackServerHeartbeat);
            sendPostRequestKafka(json);
        }

        // 恢复默认节目
        if(m_back2DefaultProgramTimeCount < m_back2DefaultProgramTime){
            m_back2DefaultProgramTimeCount++;
        }else if(m_back2DefaultProgramTimeCount == m_back2DefaultProgramTime){
            sendPostRequestOnbonDefaultProgream();
            m_back2DefaultProgramTimeCount++;
            m_triggerScreenPriority = e_triggerScreenPriority::DEFAULT;
        }

        // qDebug() << "post:" << json;
    });
    m_timer->setInterval(1000);

    m_timer->start();
}

void MyMain::init()
{
    QJsonObject cfgJson;

    if(!loadCfg(cfgJson)){
        return;
    }

    QJsonObject cfgHttpServer = cfgJson.value("HttpServer").toObject();
    QJsonObject cfgRadar = cfgJson.value("Radar").toObject();
    QJsonObject cfgI2CManager = cfgJson.value("I2CManager").toObject();
    QJsonObject cfgDetectionMode = cfgJson.value("DetectionMode").toObject();
    QJsonObject cfgGps = cfgJson.value("Gps").toObject();

    if(httpServerCfgIsOk(cfgHttpServer)){
        initHttpServer(cfgHttpServer);
    }
    if(radarCfgIsOk(cfgRadar)){
        initRadar(cfgRadar);
    }

    if(I2CManagerCfgIsOk(cfgI2CManager)){
        initI2CManager(cfgI2CManager);
    }
    if(detectionModeCfgIsOk(cfgDetectionMode)){
        initDetectionMode(cfgDetectionMode);
    }
    if(gpsCfgIsOk(cfgGps)){
        initGps(cfgGps);
    }

    if(m_radar && m_detectionMode){
        connect(m_radar, &Radar::signalUpdateDetectionModeProgram, m_detectionMode, &DetectionMode::slotUpdateDetectionModeProgram);
    }
}

bool MyMain::loadCfg(QJsonObject& cfgJson)
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

    // sbbh 配置属性是否存在
    if(cfgJson.find("sbbh") == cfgJson.end()){
        qCritical() << "error cfgJson: sbbh json miss";
        return false;
    }else if(!cfgJson.value("sbbh").isString()){
        qCritical() << "error cfgJson: sbbh not a string";
        return false;
    }
    m_data2BackServerHeartbeat.insert("sbbh", cfgJson.value("sbbh").toString());

    // heartBeatInterval 配置属性是否存在
    if(cfgJson.find("heartBeatInterval") == cfgJson.end()){
        qCritical() << "error cfgJson: heartBeatInterval json miss";
        return false;
    }else if(!cfgJson.value("heartBeatInterval").isDouble()){
        qCritical() << "error cfgJson: heartBeatInterval not a int";
        return false;
    }
    m_heartBeatInterval = cfgJson.value("heartBeatInterval").toInt();

    // back2DefaultProgramTime 配置属性是否存在
    if(cfgJson.find("back2DefaultProgramTime") == cfgJson.end()){
        qCritical() << "error cfgJson: back2DefaultProgramTime json miss";
        return false;
    }else if(!cfgJson.value("back2DefaultProgramTime").isDouble()){
        qCritical() << "error cfgJson: back2DefaultProgramTime not a int";
        return false;
    }
    m_back2DefaultProgramTime = cfgJson.value("back2DefaultProgramTime").toInt();

    // heartBeatTopic 配置属性是否存在
    if(cfgJson.find("heartBeatTopic") == cfgJson.end()){
        qCritical() << "error cfgJson: heartBeatTopic json miss";
        return false;
    }else if(!cfgJson.value("heartBeatTopic").isString()){
        qCritical() << "error cfgJson: heartBeatTopic not a string";
        return false;
    }
    m_heartBeatTopic = cfgJson.value("heartBeatTopic").toString();

    // onBonServerPort 配置属性是否存在
    if(cfgJson.find("onBonServerPort") == cfgJson.end()){
        qCritical() << "error cfgJson: onBonServerPort json miss";
        return false;
    }else if(!cfgJson.value("onBonServerPort").isDouble()){
        qCritical() << "error cfgJson: onBonServerPort not a int";
        return false;
    }
    m_onbonServerPort = cfgJson.value("onBonServerPort").toInt();

    // kafkaServerPort 配置属性是否存在
    if(cfgJson.find("kafkaServerPort") == cfgJson.end()){
        qCritical() << "error cfgJson: kafkaServerPort json miss";
        return false;
    }else if(!cfgJson.value("kafkaServerPort").isDouble()){
        qCritical() << "error cfgJson: kafkaServerPort not a int";
        return false;
    }
    m_kafkaServerPort = cfgJson.value("kafkaServerPort").toInt();

    // httpServer 配置属性是否存在
    if(cfgJson.find("HttpServer") == cfgJson.end()){
        qCritical() << "error cfgJson: httpServer json miss";
        return false;
    }else if(!cfgJson.value("HttpServer").isObject()){
        qCritical() << "error cfgJson: httpServer not a json";
        return false;
    }

    // Radar 配置属性是否存在
    if(cfgJson.find("Radar") == cfgJson.end()){
        qCritical() << "error cfgJson: Radar json miss";
        return false;
    }else if(!cfgJson.value("Radar").isObject()){
        qCritical() << "error cfgJson: Radar not a json";
        return false;
    }

    // TransmittanceMeter 配置属性是否存在
    if(cfgJson.find("I2CManager") == cfgJson.end()){
        qCritical() << "error cfgJson: I2CManager json miss";
        return false;
    }else if(!cfgJson.value("I2CManager").isObject()){
        qCritical() << "error cfgJson: I2CManager not a json";
        return false;
    }

    // DetectionMode 配置属性是否存在
    if(cfgJson.find("DetectionMode") == cfgJson.end()){
        qCritical() << "error cfgJson: DetectionMode json miss";
        return false;
    }else if(!cfgJson.value("DetectionMode").isObject()){
        qCritical() << "error cfgJson: DetectionMode not a json";
        return false;
    }
    return true;
}

bool MyMain::httpServerCfgIsOk(QJsonObject &cfgHttpServer)
{
    // 检查属性 httpServer
    if(cfgHttpServer.find("port") == cfgHttpServer.end()){
        qCritical() << "HttpServer 配置文件缺少port属性 类型int";
        return false;
    }else if(!cfgHttpServer.value("port").isDouble()){
        qCritical() << "HttpServer 配置文件port属性 类型应为int";
        return false;
    }
    int port = cfgHttpServer.value("port").toInt();
    if(port < 1023 || port > 65535){
        qCritical() << "HttpServer 配置文件port值有误 1024 ~ 65535";
        return false;
    }

    return true;
}

bool MyMain::radarCfgIsOk(QJsonObject &cfgRadar)
{
    // 检查属性 portName
    if(cfgRadar.find("portName") == cfgRadar.end()){
        qCritical() << "配置文件Radar属性内 缺少portName 类型string";
        return false;
    }else if(!cfgRadar.value("portName").isString()){
        qCritical() << "配置文件Radar属性内 portName类型错误 类型应为string";
        return false;
    }

    // 检查属性 baudRate
    if(cfgRadar.find("baudRate") == cfgRadar.end()){
        qCritical() << "配置文件Radar属性内 缺少baudRate 类型int";
        return false;
    }else if(!cfgRadar.value("baudRate").isDouble()){
        qCritical() << "配置文件Radar属性内 baudRate类型错误 类型应为int";
        return false;
    }

    int baudRate = cfgRadar.value("baudRate").toInt();
    if(baudRate != 9600 && baudRate != 19200 && baudRate != 38400
        && baudRate != 57600 && baudRate != 115200 && baudRate != 230400){
        qCritical() << "配置文件Radar属性内 baudRate值异常 9600/19200/38400/57600/115200/230400";
        return false;
    }

    // 检查属性 ownProgram
    if(cfgRadar.find("ownProgram") == cfgRadar.end()){
        qCritical() << "配置文件Radar属性内 ownProgram 类型json";
        return false;
    }else if(!cfgRadar.value("ownProgram").isObject()){
        qCritical() << "配置文件Radar属性内 ownProgram 类型错误 类型应为json";
        return false;
    }
    QJsonObject ownProgramJson = cfgRadar.value("ownProgram").toObject();

    // 检查属性 ownProgram -> normalContent
    if(ownProgramJson.find("normalContent") == ownProgramJson.end()){
        qCritical() << "配置文件Radar属性内 normalContent 类型string";
        return false;
    }else if(!ownProgramJson.value("normalContent").isString()){
        qCritical() << "配置文件Radar属性内 normalContent 类型错误 类型应为string";
        return false;
    }else if(ownProgramJson.value("normalContent").toString().size() != 4){
        qCritical() << "配置文件Radar属性内 normalContent 数量错误 只能为4个字";
        return false;
    }

    // 检查属性 ownProgram -> speedingContent
    if(ownProgramJson.find("speedingContent") == ownProgramJson.end()){
        qCritical() << "配置文件Radar属性内 speedingContent 类型string";
        return false;
    }else if(!ownProgramJson.value("speedingContent").isString()){
        qCritical() << "配置文件Radar属性内 speedingContent 类型错误 类型应为string";
        return false;
    }else if(ownProgramJson.value("speedingContent").toString().size() != 4){
        qCritical() << "配置文件Radar属性内 speedingContent 数量错误 只能为4个字";
        return false;
    }

    // 检查属性 ownProgram -> normalContentColor
    if(ownProgramJson.find("normalContentColor") == ownProgramJson.end()){
        qCritical() << "配置文件Radar属性内 缺少normalContentColor 类型int";
        return false;
    }else if(!ownProgramJson.value("normalContentColor").isDouble()){
        qCritical() << "配置文件Radar属性内 normalContentColor类型错误 类型应为int";
        return false;
    }else if(ownProgramJson.value("normalContentColor").toInt() < 1 || ownProgramJson.value("normalContentColor").toInt() > 3){
        qCritical() << "配置文件Radar属性内 normalContentColor值错误 1-红 2-绿 3-黄";
        return false;
    }

    // 检查属性 ownProgram -> speedingContentColor
    if(ownProgramJson.find("speedingContentColor") == ownProgramJson.end()){
        qCritical() << "配置文件Radar属性内 缺少 speedingContentColor 类型int";
        return false;
    }else if(!ownProgramJson.value("speedingContentColor").isDouble()){
        qCritical() << "配置文件Radar属性内 speedingContentColor 类型错误 类型应为int";
        return false;
    }else if(ownProgramJson.value("speedingContentColor").toInt() < 1 || ownProgramJson.value("speedingContentColor").toInt() > 3){
        qCritical() << "配置文件Radar属性内 speedingContentColor 值错误 1-红 2-绿 3-黄";
        return false;
    }

    // 检查属性 otherProgram
    if(cfgRadar.find("otherProgram") == cfgRadar.end()){
        qCritical() << "配置文件Radar属性内 otherProgram 类型json";
        return false;
    }else if(!cfgRadar.value("otherProgram").isObject()){
        qCritical() << "配置文件Radar属性内 otherProgram 类型错误 类型应为json";
        return false;
    }
    QJsonObject otherProgramJson = cfgRadar.value("ownProgram").toObject();


    // 检查属性 otherProgram -> content
    if(otherProgramJson.find("content") == otherProgramJson.end()){
        qCritical() << "配置文件Radar属性内 content 类型string";
        return false;
    }else if(!otherProgramJson.value("content").isString()){
        qCritical() << "配置文件Radar属性内 content 类型错误 类型应为string";
        return false;
    }else if(otherProgramJson.value("content").toString().size() != 4){
        qCritical() << "配置文件Radar属性内 content 数量错误 只能为4个字";
        return false;
    }

    // 检查属性 otherProgram -> color
    if(otherProgramJson.find("color") == otherProgramJson.end()){
        qCritical() << "配置文件Radar属性内 缺少 color 类型int";
        return false;
    }else if(!otherProgramJson.value("color").isDouble()){
        qCritical() << "配置文件Radar属性内 color 类型错误 类型应为int";
        return false;
    }else if(otherProgramJson.value("color").toInt() < 1 || otherProgramJson.value("color").toInt() > 3){
        qCritical() << "配置文件Radar属性内 color 值错误 1-红 2-绿 3-黄";
        return false;
    }


    // 检查属性 otherProgram -> signalScreen
    if(otherProgramJson.find("signalScreen") == otherProgramJson.end()){
        qCritical() << "配置文件Radar属性内 缺少 signalScreen 类型 bool";
        return false;
    }else if(!otherProgramJson.value("signalScreen").isDouble()){
        qCritical() << "配置文件Radar属性内 signalScreen 类型错误 类型应为 bool";
        return false;
    }

    // 检查属性 speedingThreshold
    if(cfgRadar.find("speedingThreshold") == cfgRadar.end()){
        qCritical() << "配置文件Radar属性内 缺少speedingThreshold 类型int";
        return false;
    }else if(!cfgRadar.value("speedingThreshold").isDouble()){
        qCritical() << "配置文件Radar属性内 speedingThreshold类型错误 类型应为int";
        return false;
    }
    return true;
}

bool MyMain::I2CManagerCfgIsOk(QJsonObject &cfgI2CManager)
{
    // 检查属性 portName
    if(cfgI2CManager.find("address") == cfgI2CManager.end()){
        qDebug() << "配置文件TransmittanceMeter属性内 address int";
        return false;
    }else if(!cfgI2CManager.value("address").isDouble()){
        qDebug() << "配置文件TransmittanceMeter属性内 address类型错误 类型应为int";
        return false;
    }
    int address = cfgI2CManager.value("address").toInt();
    if(address != 35 && address != 40){
        qDebug() << "配置文件TransmittanceMeter属性内 address值只能为 35[0x23]/64[0x40]";
        return false;
    }

    // 检查属性 baudRate
    if(cfgI2CManager.find("deviceName") == cfgI2CManager.end()){
        qDebug() << "配置文件TransmittanceMeter属性内 缺少deviceName 类型string";
        return false;
    }else if(!cfgI2CManager.value("deviceName").isString()){
        qDebug() << "配置文件TransmittanceMeterr属性内 deviceName类型错误 类型应为string";
        return false;
    }
    QString deviceName = cfgI2CManager.value("deviceName").toString();


    return true;
}

bool MyMain::detectionModeCfgIsOk(QJsonObject &cfgDetectionMode)
{
    // 检查属性 portName
    if(cfgDetectionMode.find("portName") == cfgDetectionMode.end()){
        qDebug() << "配置文件DetectionMode属性内 缺少portName 类型string";
        return false;
    }else if(!cfgDetectionMode.value("portName").isString()){
        qDebug() << "配置文件DetectionMode属性内 portName类型错误 类型应为string";
        return false;
    }

    // 检查属性 baudRate
    if(cfgDetectionMode.find("baudRate") == cfgDetectionMode.end()){
        qDebug() << "配置文件DetectionMode属性内 缺少baudRate 类型int";
        return false;
    }else if(!cfgDetectionMode.value("baudRate").isDouble()){
        qDebug() << "配置文件DetectionModer属性内 baudRate类型错误 类型应为int";
        return false;
    }
    int baudRate = cfgDetectionMode.value("baudRate").toInt();
    if(baudRate != 9600 && baudRate != 19200 && baudRate != 38400
        && baudRate != 57600 && baudRate != 115200 && baudRate != 230400){
        qCritical() << "配置文件DetectionMode属性内 baudRate值异常 9600/19200/38400/57600/115200/230400";
        return false;
    }


    // 检查属性 detectionMode
    if(cfgDetectionMode.find("detectionMode") == cfgDetectionMode.end()){
        qDebug() << "配置文件DetectionMode属性内 缺少detectionMode 类型int";
        return false;
    }else if(!cfgDetectionMode.value("detectionMode").isDouble()){
        qDebug() << "配置文件DetectionModer属性内 detectionMode类型错误 类型应为int";
        return false;
    }
    int detectionMode = cfgDetectionMode.value("detectionMode").toInt();
    if(detectionMode != 0 && detectionMode != 1 && detectionMode != 2){
        qCritical() << "配置文件DetectionMode属性内 detectionMode值异常 0-行人/1-车/3-不做多端";
        return false;
    }
    if(detectionMode == 2){
        qCritical() << "多端模块已关闭";
        return false;
    }

    return true;
}

bool MyMain::gpsCfgIsOk(QJsonObject &cfgGps)
{
    // 检查属性 portName
    if(cfgGps.find("portName") == cfgGps.end()){
        qDebug() << "配置文件Gps属性内 缺少portName 类型string";
        return false;
    }else if(!cfgGps.value("portName").isString()){
        qDebug() << "配置文件Gps属性内 portName类型错误 类型应为string";
        return false;
    }

    // 检查属性 baudRate
    if(cfgGps.find("baudRate") == cfgGps.end()){
        qDebug() << "配置文件Gps属性内 缺少baudRate 类型int";
        return false;
    }else if(!cfgGps.value("baudRate").isDouble()){
        qDebug() << "配置文件Gps属性内 baudRate类型错误 类型应为int";
        return false;
    }
    int baudRate = cfgGps.value("baudRate").toInt();
    if(baudRate != 9600 && baudRate != 19200 && baudRate != 38400
        && baudRate != 57600 && baudRate != 115200 && baudRate != 230400){
        qCritical() << "配置文件Gps属性内 baudRate值异常 9600/19200/38400/57600/115200/230400";
        return false;
    }
    return true;
}

void MyMain::initHttpServer(QJsonObject &cfgHttpServer)
{
    m_httpserver = new MyHttpServer(cfgHttpServer, this);
    connect(m_httpserver, &MyHttpServer::signalOpenControl, this, &MyMain::slotOpenControl);
}

void MyMain::initRadar(QJsonObject &cfgRadar)
{
    m_radar = new Radar(cfgRadar, this);
    connect(m_radar, &Radar::signalSendSpeedProgram2OnbonUp, this, &MyMain::slotSendSpeedProgram2OnbonUp);
    connect(m_radar, &Radar::signalSendSpeedProgram2OnbonDown, this, &MyMain::slotSendSpeedProgram2OnbonDown);
}

void MyMain::initI2CManager(QJsonObject &cfgI2CManager)
{
    m_I2CManager = new I2CManager(cfgI2CManager, this);
    connect(m_I2CManager, &I2CManager::signalUpdateI2CData, this, [this](QByteArray data){
        QJsonObject json = QJsonDocument::fromJson(data).object();
        m_data2BackServerHeartbeat["busVoltage"] = json.value("busVoltage").toString();
        m_data2BackServerHeartbeat["shuntVoltage"] = json.value("shuntVoltage").toString();
        m_data2BackServerHeartbeat["current"] = json.value("current").toString();
        m_data2BackServerHeartbeat["power"] = json.value("power").toString();
        m_data2BackServerHeartbeat["Lux"] = json.value("Lux").toString();
    });

    m_I2CManager->start();
}

void MyMain::initDetectionMode(QJsonObject &cfgDetectionMode)
{
    m_detectionMode = new DetectionMode(cfgDetectionMode, this);
    connect(m_detectionMode, &DetectionMode::signalPlayOtherLuaProgram, this, &MyMain::slotPlayOtherLuaProgram);
}

void MyMain::initGps(QJsonObject &cfgGps)
{
    m_gps = new GPS(cfgGps, this);
}

void MyMain::sendPostRequestKafka(QJsonObject &json)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager();

    // 设置请求 URL 和 headers
    QNetworkRequest request;
    request.setUrl(QUrl(QString("http://127.0.0.1:%1/kafka/producer").arg(m_kafkaServerPort)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    qDebug() << json;
    // 发送 POST 请求
    QNetworkReply *reply = manager->post(request, QJsonDocument(json).toJson());

    // 异步处理响应
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "Response:" << reply->readAll();
        } else {
            qDebug() << "Error:" << reply->errorString();
        }
        reply->deleteLater();
        manager->deleteLater();
    });
}

void MyMain::sendPostRequestOnbon(QJsonObject &json)
{
    // 重置恢复默认节目的计数
    m_back2DefaultProgramTimeCount = 0;

    QNetworkAccessManager *manager = new QNetworkAccessManager();

    // 设置请求 URL 和 headers
    QNetworkRequest request;
    request.setUrl(QUrl(QString("http://127.0.0.1:%1/screen/text2Dynamic").arg(m_onbonServerPort)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // qDebug() << json;
    // 发送 POST 请求
    QNetworkReply *reply = manager->post(request, QJsonDocument(json).toJson());

    // 异步处理响应
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            // qDebug() << "Response:" << reply->readAll();
            qDebug() << "Response: success";
        } else {
            // qDebug() << "Error:" << reply->errorString();
            qDebug() << "Error: failed";
        }
        reply->deleteLater();
        manager->deleteLater();
    });
}

void MyMain::sendPostRequestOnbonDefaultProgream()
{
    QJsonObject jsonData;
    jsonData.insert("nBaudRateIndex", 2);
    jsonData.insert("areaId", 255);

    QNetworkAccessManager *manager = new QNetworkAccessManager();

    // 设置请求 URL 和 headers
    QNetworkRequest request;
    request.setUrl(QUrl(QString("http://127.0.0.1:%1/screen/closeDynamic").arg(m_onbonServerPort)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // qDebug() << json;
    // 发送 POST 请求
    QNetworkReply *reply = manager->post(request, QJsonDocument(jsonData).toJson());

    // 异步处理响应
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            // qDebug() << "Response:" << reply->readAll();
            qDebug() << "Response: success";
        } else {
            // qDebug() << "Error:" << reply->errorString();
            qDebug() << "Error: failed";
        }
        reply->deleteLater();
        manager->deleteLater();
    });
}

void MyMain::slotOpenControl(QByteArray jsonData, bool open)
{
    // 关闭管控
    if(!open){
        m_triggerScreenPriority = e_triggerScreenPriority::DEFAULT;
        m_back2DefaultProgramTimeCount = m_back2DefaultProgramTime;
        m_data2BackServerHeartbeat["controlState"] = false;
        return;
    }

    // 开启管控
    QJsonObject json = QJsonDocument::fromJson(jsonData).object();
    m_triggerScreenPriority = e_triggerScreenPriority::CONTROL;
    m_back2DefaultProgramTimeCount = m_back2DefaultProgramTime+1;   // 管控不回默认节目
    sendPostRequestOnbon(json);
    m_data2BackServerHeartbeat["controlState"] = true;

    // e_triggerScreenPriority Priority = (e_triggerScreenPriority)json.value("Priority").toInt();
    // if(m_triggerScreenPriority > Priority){
    //     m_triggerScreenPriority = Priority;
    //     m_back2DefaultProgramTimeCount = 0;
    //     sendPostRequestOnbon(json);
    // }
}

void MyMain::slotPlayOtherLuaProgram(QByteArray jsonData)
{
    if(m_triggerScreenPriority < e_triggerScreenPriority::OTHER_LUA){
        return;
    }

    QJsonObject json = QJsonDocument::fromJson(jsonData).object();
    QString content = json.value("content").toString();
    int fontSize = 1;
    int contentSize = content.size();
    bool signalScreen = json.value("signalScreen").toBool();
    QString left, right;

    if(signalScreen){
        switch(contentSize){
        case 2:
            fontSize = 48;
            content.insert(1, "  ");
            break;
        case 4:
            fontSize = 45;
            break;
        case 5:
            fontSize = 36;
            break;
        case 6:
            fontSize = 29;
            break;
        case 7:
            fontSize = 25;
            break;
        case 8:
            for(int i = 0; i<contentSize; i++){
                content.insert(i + i*1, "\n");
            }
            content = content.trimmed();
            fontSize = 22;
            break;
        case 12:
            left = content.left(content.size()/2);
            right = content.right(content.size()/2);
            content.clear();
            for (int i=0; i<left.size(); i++) {
                content += left.at(i);
                content += right.at(i);
            }
            fontSize = 24;
            break;
        case 16:
            left = content.left(content.size()/2);
            right = content.right(content.size()/2);
            content.clear();
            for (int i=0; i<left.size(); i++) {
                content += left.at(i);
                content += right.at(i);
            }
            fontSize = 22;
            break;
        default:
            break;
        }
    }else{
        switch(contentSize){
        case 4:
            fontSize = 48;
            content.insert(3, "  ");
            content.insert(1, "  ");
            break;
        case 8:
            fontSize = 45;
            break;
        case 10:
            fontSize = 36;
            break;
        case 12:
            fontSize = 29;
            break;
        case 14:
            fontSize = 25;
            break;
        default:
            break;
        }
    }

    QJsonObject jsonDataP;
    jsonDataP.insert("nBaudRateIndex", 2);
    jsonDataP.insert("color", json.value("color").toInt());
    jsonDataP.insert("areaId", 0);
    jsonDataP.insert("areaX", 0);
    jsonDataP.insert("areaY", 0);
    jsonDataP.insert("areaWidth", 64);
    jsonDataP.insert("areaHeight", 256);
    jsonDataP.insert("fontName", "等线");
    jsonDataP.insert("fontSize", fontSize);
    jsonDataP.insert("content", content);
    jsonDataP.insert("DisplayMode", 1);
    jsonDataP.insert("Speed", 1);

    m_back2DefaultProgramTimeCount = 0;   // 超时回默认节目
    sendPostRequestOnbon(json);
    m_triggerScreenPriority = e_triggerScreenPriority::OTHER_LUA;
}

// void MyMain::slotPlayOtherLuaProgram(QByteArray jsonData)
// {
//     QJsonObject json = QJsonDocument::fromJson(jsonData).object();
//     if(m_triggerScreenPriority >= e_triggerScreenPriority::OTHER_LUA){
//         m_triggerScreenPriority = e_triggerScreenPriority::OTHER_LUA;
//         m_back2DefaultProgramTimeCount = m_back2DefaultProgramTime+1;
//         sendPostRequestOnbon(json);
//     }
// }

void MyMain::slotSendSpeedProgram2OnbonUp(QString speed, int color)
{
    if(m_triggerScreenPriority < e_triggerScreenPriority::RADAR){
        return;
    }
    // 35 30 速度字号
    //  0 0 64 42 居中
    //qDebug() << "speed: " << speed << " color: " << color;
    QJsonObject jsonData;
    jsonData.insert("nBaudRateIndex", 2);
    jsonData.insert("color", color);
    jsonData.insert("areaId", 0);
    jsonData.insert("areaX", 0);
    jsonData.insert("areaY", 0);
    jsonData.insert("areaWidth", 64);
    jsonData.insert("areaHeight", 42);
    jsonData.insert("fontName", "等线");
    jsonData.insert("fontSize", speed.size() == 2 ? 35 : 30);
    jsonData.insert("content", speed);
    jsonData.insert("Halign", 2);
    jsonData.insert("Valign", 2);
    jsonData.insert("DisplayMode", 1);
    jsonData.insert("Speed", 1);
    sendPostRequestOnbon(jsonData);
    m_triggerScreenPriority = e_triggerScreenPriority::RADAR;
}

void MyMain::slotSendSpeedProgram2OnbonDown(QString content, int color)
{
    // 如果当前优先级高于雷达 直接返回
    if(m_triggerScreenPriority < e_triggerScreenPriority::RADAR){
        return;
    }
    // 0 48 64 208 字号36 居中
    QJsonObject jsonData;
    jsonData.insert("nBaudRateIndex", 2);
    jsonData.insert("color", color);
    jsonData.insert("areaId", 1);
    jsonData.insert("areaX", 0);
    jsonData.insert("areaY", 48);
    jsonData.insert("areaWidth", 64);
    jsonData.insert("areaHeight", 208);
    jsonData.insert("fontName", "等线");
    jsonData.insert("fontSize", 36);
    jsonData.insert("content", content);
    jsonData.insert("Halign", 2);
    jsonData.insert("Valign", 2);
    jsonData.insert("DisplayMode", 1);
    jsonData.insert("Speed", 1);
    sendPostRequestOnbon(jsonData);
    m_triggerScreenPriority = e_triggerScreenPriority::RADAR;
}

void MyMain::slotUpdateGpsNE(QString N, QString E)
{
    QJsonArray jsonArray;
    jsonArray << N << E;
    m_data2BackServerHeartbeat["gps"] = jsonArray;
}
