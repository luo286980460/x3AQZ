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

#define CFG_JSON            "/cfg.json"

// screen
#define TEXT_2_DYNAMIC      "/screen/text2Dynamic"      // 发送动态区节目接口
#define TEXT_2_STATIC       "/screen/text2Static"       // 发送静态区节目接口
#define SET_BRIGHTTNESS     "/screen/setBrightness"     // 更新亮度

// aPlayer
#define APLAYER_SET_VOLUME  "/setVolume"                // 设置音量
#define APLAYER_SET_LOOP    "/setLoop"                  // 设置循环模式
#define APLAYER_ADD_AUDIOS  "/addAudio"                 // 添加文件到播放列表
#define APLAYER_DEL_AUDIOS  "/clearPlayList"            // 清空播放列表
#define APLAYER_PLAY_AUDIO  "/playIndex"                // 根据下标播放音频
#define APLAYER_STOP        "/play/stop"                // 根据下标播放音频

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
    m_timer = new QTimer(this);
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
            setTriggerScreenPriority(e_triggerScreenPriority::DEFAULT_PROGRAM);

            // 关闭语音播放
            m_stopAudioLagCount = 0;
        }

        // 延迟关闭语音
        //qDebug() << "m_stopAudioLagCount : " << m_stopAudioLagCount << " .. " << m_stopAudioLagValue;
        if(m_stopAudioLagCount == m_stopAudioLagValue){
            // 发送语音节目
            QJsonObject jsonDataAudio;
            sendPostRequestAPlayer(jsonDataAudio, APLAYER_STOP);
            m_stopAudioLagCount++;
        }else if(m_stopAudioLagCount < m_stopAudioLagValue){
            m_stopAudioLagCount++;
        }

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
    QJsonObject cfgAPlayer = cfgJson.value("APlayer").toObject();

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

    if(aPlayerCfgIsOk(cfgAPlayer)){
        initAPlayer(cfgAPlayer);
    }

    if(m_radar && m_detectionMode){
        connect(m_radar, &Radar::signalUpdateDetectionModeProgram, m_detectionMode, &DetectionMode::slotUpdateDetectionModeProgram);
    }

    // 清空节目 然后 更新默认节目
    sendPostRequestOnbonDefaultProgream();
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

    // aPlayerServerPort 配置属性是否存在
    if(cfgJson.find("aPlayerServerPort") == cfgJson.end()){
        qCritical() << "error cfgJson: aPlayerServerPort json miss";
        return false;
    }else if(!cfgJson.value("aPlayerServerPort").isDouble()){
        qCritical() << "error cfgJson: aPlayerServerPort not a int";
        return false;
    }
    m_aPlayerServerPort = cfgJson.value("aPlayerServerPort").toInt();

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

    // speedingAudioId 配置属性是否存在
    if(cfgJson.find("speedingAudioId") == cfgJson.end()){
        qCritical() << "error cfgJson: speedingAudioId json miss";
        return false;
    }else if(!cfgJson.value("speedingAudioId").isDouble()){
        qCritical() << "error cfgJson: speedingAudioId not a int";
        return false;
    }
    m_speedingAudioId = cfgJson.value("speedingAudioId").toInt();

    // speedingAudioIdTimes 配置属性是否存在
    if(cfgJson.find("speedingAudioIdTimes") == cfgJson.end()){
        qCritical() << "error cfgJson: speedingAudioIdTimes json miss";
        return false;
    }else if(!cfgJson.value("speedingAudioIdTimes").isDouble()){
        qCritical() << "error cfgJson: speedingAudioIdTimes not a int";
        return false;
    }
    m_speedingAudioIdTimes = cfgJson.value("speedingAudioIdTimes").toInt();

    // luraAudioId 配置属性是否存在
    if(cfgJson.find("luraAudioId") == cfgJson.end()){
        qCritical() << "error cfgJson: luraAudioId json miss";
        return false;
    }else if(!cfgJson.value("luraAudioId").isDouble()){
        qCritical() << "error cfgJson: luraAudioId not a int";
        return false;
    }
    m_luraAudioId = cfgJson.value("luraAudioId").toInt();

    // luraAudioIdTimes 配置属性是否存在
    if(cfgJson.find("luraAudioIdTimes") == cfgJson.end()){
        qCritical() << "error cfgJson: luraAudioIdTimes json miss";
        return false;
    }else if(!cfgJson.value("luraAudioIdTimes").isDouble()){
        qCritical() << "error cfgJson: luraAudioIdTimes not a int";
        return false;
    }
    m_luraAudioIdTimes = cfgJson.value("luraAudioIdTimes").toInt();

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
    // 检查属性 cfgRadar-> work
    if(cfgRadar.find("work") == cfgRadar.end()){
        qCritical() << "配置文件Radar->otherProgram 属性内 缺少 work 类型 bool";
        return false;
    }else if(!cfgRadar.value("work").isBool()){
        qCritical() << "配置文件Radar->otherProgram 属性内 work 类型错误 类型应为 bool";
        return false;
    }

    if(!cfgRadar.value("work").toBool()){
        qCritical() << "雷达模块不器用";
        return false;
    }

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
        qCritical() << "配置文件Radar->ownProgram 属性内 normalContent 类型string";
        return false;
    }else if(!ownProgramJson.value("normalContent").isString()){
        qCritical() << "配置文件Radar->ownProgram 属性内 normalContent 类型错误 类型应为string";
        return false;
    }else if(ownProgramJson.value("normalContent").toString().size() != 4){
        qCritical() << "配置文件Radar->ownProgram 属性内 normalContent 数量错误 只能为4个字";
        return false;
    }

    // 检查属性 ownProgram -> speedingContent
    if(ownProgramJson.find("speedingContent") == ownProgramJson.end()){
        qCritical() << "配置文件Radar->ownProgram 属性内 speedingContent 类型string";
        return false;
    }else if(!ownProgramJson.value("speedingContent").isString()){
        qCritical() << "配置文件Radar->ownProgram 属性内 speedingContent 类型错误 类型应为string";
        return false;
    }else if(ownProgramJson.value("speedingContent").toString().size() != 4){
        qCritical() << "配置文件Radar->ownProgram 属性内 speedingContent 数量错误 只能为4个字";
        return false;
    }

    // 检查属性 ownProgram -> normalContentColor
    if(ownProgramJson.find("normalContentColor") == ownProgramJson.end()){
        qCritical() << "配置文件Radar->ownProgram 属性内 缺少normalContentColor 类型int";
        return false;
    }else if(!ownProgramJson.value("normalContentColor").isDouble()){
        qCritical() << "配置文件Radar->ownProgram 属性内 normalContentColor类型错误 类型应为int";
        return false;
    }else if(ownProgramJson.value("normalContentColor").toInt() < 1 || ownProgramJson.value("normalContentColor").toInt() > 3){
        qCritical() << "配置文件Radar->ownProgram 属性内 normalContentColor值错误 1-红 2-绿 3-黄";
        return false;
    }

    // 检查属性 ownProgram -> speedingContentColor
    if(ownProgramJson.find("speedingContentColor") == ownProgramJson.end()){
        qCritical() << "配置文件Radar->ownProgram 属性内 缺少 speedingContentColor 类型int";
        return false;
    }else if(!ownProgramJson.value("speedingContentColor").isDouble()){
        qCritical() << "配置文件Radar->ownProgram 属性内 speedingContentColor 类型错误 类型应为int";
        return false;
    }else if(ownProgramJson.value("speedingContentColor").toInt() < 1 || ownProgramJson.value("speedingContentColor").toInt() > 3){
        qCritical() << "配置文件Radar->ownProgram 属性内 speedingContentColor 值错误 1-红 2-绿 3-黄";
        return false;
    }

    // 检查属性 otherProgram
    if(cfgRadar.find("otherProgram") == cfgRadar.end()){
        qCritical() << "配置文件Radar 属性内 otherProgram 类型json";
        return false;
    }else if(!cfgRadar.value("otherProgram").isObject()){
        qCritical() << "配置文件Radar 属性内 otherProgram 类型错误 类型应为json";
        return false;
    }
    QJsonObject otherProgramJson = cfgRadar.value("otherProgram").toObject();


    // 检查属性 otherProgram -> content
    if(otherProgramJson.find("content") == otherProgramJson.end()){
        qCritical() << "配置文件Radar->otherProgram 属性内 content 类型string";
        return false;
    }else if(!otherProgramJson.value("content").isString()){
        qCritical() << "配置文件Radar->otherProgram 属性内 content 类型错误 类型应为string";
        return false;
    }else if(otherProgramJson.value("content").toString().size() != 4){
        qCritical() << "配置文件Radar->otherProgram 属性内 content 数量错误 只能为4个字";
        return false;
    }

    // 检查属性 otherProgram -> color
    if(otherProgramJson.find("color") == otherProgramJson.end()){
        qCritical() << "配置文件Radar->otherProgram 属性内 缺少 color 类型int";
        return false;
    }else if(!otherProgramJson.value("color").isDouble()){
        qCritical() << "配置文件Radar->otherProgram 属性内 color 类型错误 类型应为int";
        return false;
    }else if(otherProgramJson.value("color").toInt() < 1 || otherProgramJson.value("color").toInt() > 3){
        qCritical() << "配置文件Radar->otherProgram 属性内 color 值错误 1-红 2-绿 3-黄";
        return false;
    }


    // 检查属性 otherProgram -> signalScreen
    if(otherProgramJson.find("singleScreen") == otherProgramJson.end()){
        qCritical() << "配置文件Radar->otherProgram 属性内 缺少 singleScreen 类型 bool";
        return false;
    }else if(!otherProgramJson.value("singleScreen").isBool()){
        qCritical() << "配置文件Radar->otherProgram 属性内 singleScreen 类型错误 类型应为 bool";
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
    // 检查属性 interval
    if(cfgI2CManager.find("interval") == cfgI2CManager.end()){
        qDebug() << "配置文件 I2CManager 属性内 interval int";
        return false;
    }else if(!cfgI2CManager.value("interval").isDouble()){
        qDebug() << "配置文件 I2CManager 属性内 interval 类型错误 类型应为 int";
        return false;
    }

    // 检查属性 address
    if(cfgI2CManager.find("address") == cfgI2CManager.end()){
        qDebug() << "配置文件 I2CManager 属性内 address int";
        return false;
    }else if(!cfgI2CManager.value("address").isDouble()){
        qDebug() << "配置文件 I2CManager 属性内 address类型错误 类型应为int";
        return false;
    }
    int address = cfgI2CManager.value("address").toInt();
    if(address != 35 && address != 40){
        qDebug() << "配置文件 I2CManager 属性内 address 值只能为 35[0x23]/64[0x40]";
        return false;
    }

    // 检查属性 deviceName
    if(cfgI2CManager.find("deviceName") == cfgI2CManager.end()){
        qDebug() << "配置文件 I2CManager 属性内 缺少 deviceName 类型string";
        return false;
    }else if(!cfgI2CManager.value("deviceName").isString()){
        qDebug() << "配置文件 I2CManager 属性内 deviceName 类型错误 类型应为string";
        return false;
    }
    QString deviceName = cfgI2CManager.value("deviceName").toString();


    // 检查属性 luxLevel
    if(cfgI2CManager.find("luxLevel") == cfgI2CManager.end()){
        qDebug() << "配置文件 I2CManager 属性内 缺少 luxLevel 类型 jsonArray";
        return false;
    }else if(!cfgI2CManager.value("luxLevel").isArray()){
        qDebug() << "配置文件 I2CManager 属性内 luxLevel 类型错误 类型应为 jsonArray";
        return false;
    }
    QJsonArray luxLevelArray = cfgI2CManager.value("luxLevel").toArray();
    foreach (QJsonValue luxLevel, luxLevelArray) {
        if(!luxLevel.isObject()){
            qDebug() << "配置文件 I2CManager->luxLevel->value 属性 类型错误 类型应为 json";
            return false;
        }
        QJsonObject luxLevelJson = luxLevel.toObject();
        if(!luxLevelJson.value("volume").isDouble()){
            qDebug() << "配置文件 I2CManager->luxLevel->json->volume 属性 类型错误 类型应为 int";
            return false;
        }else if(!luxLevelJson.value("screenBrightness").isDouble()){
            qDebug() << "配置文件 I2CManager->luxLevel->json->screenBrightness 属性 类型错误 类型应为 int";
            return false;
        }

        int volume = luxLevelJson.value("volume").toInt();
        if(volume < 1 || volume > 10){
            qDebug() << "配置文件 I2CManager->luxLevel->json->volume 属性 值错误 1~10";
            return false;
        }

        int screenBrightness = luxLevelJson.value("screenBrightness").toInt();
        if(screenBrightness < 0 || screenBrightness > 15){
            qDebug() << "配置文件 I2CManager->luxLevel->json->screenBrightness 属性 值错误 0~15";
            return false;
        }
    }

    // 检查属性 times
    if(cfgI2CManager.find("times") == cfgI2CManager.end()){
        qDebug() << "配置文件 I2CManager 属性内 times int";
        return false;
    }else if(!cfgI2CManager.value("times").isDouble()){
        qDebug() << "配置文件 I2CManager 属性内 times 类型错误 类型应为 int";
        return false;
    }

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
        qCritical() << "配置文件DetectionMode属性内 detectionMode值异常 0-主机/1-从机/2-不做多端";
        return false;
    }
    if(detectionMode == 2){
        qCritical() << "多端模块已关闭";
        return false;
    }

    // 检查属性 loopTimer
    if(cfgDetectionMode.find("loopTimer") == cfgDetectionMode.end()){
        qDebug() << "配置文件 DetectionMode 属性内 缺少 loopTimer 类型int";
        return false;
    }else if(!cfgDetectionMode.value("loopTimer").isDouble()){
        qDebug() << "配置文件 DetectionMode 属性内 loopTimer 类型错误 类型应为int";
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

bool MyMain::aPlayerCfgIsOk(QJsonObject &cfgAPlayer)
{
    // 检查属性 port
    if(cfgAPlayer.find("port") == cfgAPlayer.end()){
        qDebug() << "配置文件 APlayer 属性内 缺少 port 类型int";
        return false;
    }else if(!cfgAPlayer.value("port").isDouble()){
        qDebug() << "配置文件 APlayer 属性内 port 类型错误 类型应为int";
        return false;
    }
    int port = cfgAPlayer.value("port").toInt();
    if(port < 1000 || port > 65535){
        qCritical() << "配置文件 APlayer 属性内 port 值异常 1000~65535";
        return false;
    }

    // 检查属性 playList
    if(cfgAPlayer.find("playList") == cfgAPlayer.end()){
        qDebug() << "配置文件 APlayer 属性内 缺少 playList 类型 array";
        return false;
    }else if(!cfgAPlayer.value("port").isDouble()){
        qDebug() << "配置文件 APlayer 属性内 playList 类型错误 类型应为 array";
        return false;
    }
    QJsonArray playList = cfgAPlayer.value("playList").toArray();
    for (int i = 0; i < playList.size(); ++i) {
        if(!playList.at(i).isString()){
            qDebug() << "配置文件 APlayer->playList  类型错误 类型应为 stringList";
            return false;
        }
    }

    return true;
}

void MyMain::initHttpServer(QJsonObject &cfgHttpServer)
{
    m_httpserver = new MyHttpServer(cfgHttpServer, this);
    connect(m_httpserver, &MyHttpServer::signalOpenControl, this, &MyMain::slotOpenControl);
    connect(m_httpserver, &MyHttpServer::signalSetDefaultProgam, this, &MyMain::slotSetDefaultProgam);
    SetDefaultProgam(cfgHttpServer);
}

void MyMain::initRadar(QJsonObject &cfgRadar)
{
    m_radar = new Radar(cfgRadar, this);
    connect(m_radar, &Radar::signalSendSpeedProgram2OnbonUp, this, &MyMain::slotSendSpeedProgram2OnbonUp);
    connect(m_radar, &Radar::signalSendSpeedProgram2OnbonDown, this, &MyMain::slotSendSpeedProgram2OnbonDown);
    connect(m_radar, &Radar::signalPlaySpeedingAudio, this, &MyMain::slotPlaySpeedingAudio);
    connect(m_radar, &Radar::signalStopSpeedingAudio, this, &MyMain::slotStopSpeedingAudio);
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

    connect(m_I2CManager, &I2CManager::signalUpdateVolumeAndBrightness, this, &MyMain::slotUpdateVolumeAndBrightness);

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

void MyMain::initAPlayer(QJsonObject &cfgAPlayer)
{
    // 将可播放内容添加到播放列表
    QJsonObject json;
    json.insert("data", cfgAPlayer.value("playList").toArray());

    sendPostRequestAPlayer(json, APLAYER_DEL_AUDIOS);

    sendPostRequestAPlayer(json, APLAYER_ADD_AUDIOS);
}

void MyMain::sendPostRequestKafka(QJsonObject &json)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    // 设置请求 URL 和 headers
    QNetworkRequest request;
    request.setUrl(QUrl(QString("http://127.0.0.1:%1/kafka/producer").arg(m_kafkaServerPort)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // qDebug() << json;
    // 发送 POST 请求
    QNetworkReply *reply = manager->post(request, QJsonDocument(json).toJson());

    // 异步处理响应
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            // qDebug() << "Response:" << reply->readAll();
            // qDebug() << "Response: success";
        } else {
            qDebug() << "Error:" << reply->errorString();
        }
        reply->deleteLater();
        manager->deleteLater();
    });
}

void MyMain::sendPostRequestOnbon(QJsonObject &json, QString api)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    // 设置请求 URL 和 headers
    QNetworkRequest request;
    request.setUrl(QUrl(QString("http://127.0.0.1:%1%2").arg(m_onbonServerPort).arg(api)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // qDebug() << json;
    // 发送 POST 请求
    QNetworkReply *reply = manager->post(request, QJsonDocument(json).toJson());

    // 异步处理响应
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            // qDebug() << "Response:" << reply->readAll();
            // qDebug() << "Response: success";
        } else {
            // qDebug() << "Error:" << reply->errorString();
            qDebug() << "Error: failed";
        }
        reply->deleteLater();
        manager->deleteLater();
    });
}

void MyMain::sendPostRequestAPlayer(QJsonObject &json, QString api)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    // 设置请求 URL 和 headers
    QNetworkRequest request;
    request.setUrl(QUrl(QString("http://127.0.0.1:%1%2").arg(m_aPlayerServerPort).arg(api)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // qDebug() << json;
    // 发送 POST 请求
    QNetworkReply *reply = manager->post(request, QJsonDocument(json).toJson());

    // 异步处理响应
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            // qDebug() << "Response:" << reply->readAll();
            // qDebug() << "Response: success";
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
    // 恢复默认节目
    QJsonObject jsonData;
    jsonData.insert("nBaudRateIndex", 2);
    jsonData.insert("areaId", 255);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

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
            // qDebug() << "Response: success";
        } else {
            // qDebug() << "Error:" << reply->errorString();
            qDebug() << "Error: failed";
        }
        reply->deleteLater();
        manager->deleteLater();
    });
}

void MyMain::SetDefaultProgam(QJsonObject &cfgHttpServer)
{
    QJsonObject backJson;
    backJson.insert("error", 1);
    QJsonObject jsonProgram = cfgHttpServer.value("defaultProgram").toObject();
    QJsonObject jsonProgramData;
    if(m_httpserver){
        jsonProgramData = m_httpserver->parseScreenOpenControl(jsonProgram, backJson);
    }

    sendPostRequestOnbon(jsonProgramData, TEXT_2_STATIC);
}

void MyMain::setTriggerScreenPriority(e_triggerScreenPriority Priority)
{
    m_triggerScreenPriority = Priority;
    if(m_radar){
        m_radar->setPriority(Priority);
    }
}

void MyMain::slotOpenControl(QByteArray jsonData, bool open)
{
    // 关闭管控
    if(!open){
        setTriggerScreenPriority(e_triggerScreenPriority::DEFAULT_PROGRAM);
        // 设置默认节目的计数
        m_back2DefaultProgramTimeCount = m_back2DefaultProgramTime;

        m_controlState = false;
        m_heartBeatCount = m_heartBeatInterval-1;
        return;
    }

    // 开启管控
    QJsonObject json = QJsonDocument::fromJson(jsonData).object();

    setTriggerScreenPriority(e_triggerScreenPriority::CONTROL);
    sendPostRequestOnbonDefaultProgream();
    sendPostRequestOnbon(json, TEXT_2_DYNAMIC);
    m_controlState = true;

    m_heartBeatCount = m_heartBeatInterval-1;
    m_back2DefaultProgramTimeCount = m_back2DefaultProgramTime+1;   // 管控不回默认节目
}

void MyMain::slotSetDefaultProgam(QByteArray jsonData)
{
    QJsonObject json = QJsonDocument::fromJson(jsonData).object();
    sendPostRequestOnbon(json, TEXT_2_STATIC);
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

    // 发送动态区节目
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
    jsonDataP.insert("Halign", 2);
    jsonDataP.insert("Valign", 2);
    jsonDataP.insert("DisplayMode", 1);
    jsonDataP.insert("Speed", 1);

    // 重置恢复默认节目的计数
    m_back2DefaultProgramTimeCount = 0;

    sendPostRequestOnbon(json, TEXT_2_DYNAMIC);

    // 发送语音节目
    QJsonObject jsonDataAudio;
    jsonDataAudio.insert("index", m_luraAudioId);
    jsonDataAudio.insert("times", m_luraAudioIdTimes);
    sendPostRequestAPlayer(jsonDataAudio, APLAYER_PLAY_AUDIO);

    // 设置优先级
    setTriggerScreenPriority(e_triggerScreenPriority::OTHER_LUA);
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
    sendPostRequestOnbon(jsonData, TEXT_2_DYNAMIC);

    // 重置恢复默认节目的计数
    m_back2DefaultProgramTimeCount = 0;

    setTriggerScreenPriority(e_triggerScreenPriority::RADAR);
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
    sendPostRequestOnbon(jsonData, TEXT_2_DYNAMIC);

    setTriggerScreenPriority(e_triggerScreenPriority::RADAR);
}

void MyMain::slotUpdateGpsNE(QString N, QString E)
{
    QJsonArray jsonArray;
    jsonArray << N << E;
    m_data2BackServerHeartbeat["gps"] = jsonArray;
}

void MyMain::slotUpdateVolumeAndBrightness(int volume, int screenBrightness)
{
    QJsonObject json;
    json.insert("nBaudRateIndex", 2);
    json.insert("brightness", screenBrightness);

    if(screenBrightness != m_lastScreenBrightness){
        sendPostRequestOnbon(json, SET_BRIGHTTNESS);
    }

    json = {};

    json.insert("volume", volume);
    if(volume != m_lastVolume){
        // sendPostRequestOnbon(json, SET_BRIGHTTNESS);
        sendPostRequestAPlayer(json, APLAYER_SET_VOLUME);
    }
}

void MyMain::slotPlaySpeedingAudio()
{
    // 发送语音节目
    QJsonObject jsonDataAudio;
    jsonDataAudio.insert("index", m_speedingAudioId);
    jsonDataAudio.insert("times", m_speedingAudioIdTimes);
    sendPostRequestAPlayer(jsonDataAudio, APLAYER_PLAY_AUDIO);
    m_stopAudioLagCount = m_stopAudioLagValue+1;
}

void MyMain::slotStopSpeedingAudio()
{
    m_stopAudioLagCount = 0;
}
