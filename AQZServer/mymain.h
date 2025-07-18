#ifndef MYMAIN_H
#define MYMAIN_H

#include <QObject>
#include <QJsonObject>
#include <QTimer>
#include <QByteArray>

class MyHttpServer;
class Producer;
class Radar;
class I2CManager;
class DetectionMode;
class GPS;

// 节目触发优先级
typedef enum {
    DEBUG,      // 调试触发
    CONTROL,    // 管控触发
    OTHER_LUA,  // 他端lua触发
    RADAR,      // 本地雷达触发
    DEFAULT_PROGRAM,    // 默认节目
    ALL
}e_triggerScreenPriority;

class MyMain : public QObject
{
    Q_OBJECT
public:
    explicit MyMain(QObject *parent = nullptr);

private:
    void initTimer();
    // void initTimer1();
    void init();
    bool loadCfg(QJsonObject& cfgJson);
    bool httpServerCfgIsOk(QJsonObject &cfgHttpServer);
    bool radarCfgIsOk(QJsonObject& cfgRadar);
    bool I2CManagerCfgIsOk(QJsonObject& cfgI2CManager);
    bool detectionModeCfgIsOk(QJsonObject& cfgDetectionMode);
    bool gpsCfgIsOk(QJsonObject& cfgGps);
    void initHttpServer(QJsonObject& cfgHttpServer);
    void initRadar(QJsonObject& cfgRadar);
    void initI2CManager(QJsonObject& cfgI2CManager);
    void initDetectionMode(QJsonObject& cfgDetectionMode);
    void initGps(QJsonObject& cfgGps);
    void sendPostRequestKafka(QJsonObject& json);
    void sendPostRequestOnbon(QJsonObject& json, QString api);
    void sendPostRequestOnbonDefaultProgream();     // 返回默认节目(清空动态区)
    void SetDefaultProgam(QJsonObject& cfgHttpServer);  // 设置默认节目信息


signals:
    void signalWrite2Kafka(QString strJson, QString strKey);

public slots:
    void slotOpenControl(QByteArray jsonData, bool open);           // 开启管控
    void slotSetDefaultProgam(QByteArray jsonData);                 // 设置默认节目信息
    void slotPlayOtherLuaProgram(QByteArray jsonData);              // 第三lura方激活的节目
    void slotSendSpeedProgram2OnbonUp(QString speed, int color);    // 速度节目的数字(屏幕上半部分)
    void slotSendSpeedProgram2OnbonDown(QString content, int color);// 速度节目的文字(屏幕下半部分)
    void slotUpdateGpsNE(QString N, QString E);                     // 更新心跳包内位置信息

private:
    MyHttpServer* m_httpserver = nullptr;
    Radar* m_radar = nullptr;
    I2CManager* m_I2CManager = nullptr;
    DetectionMode* m_detectionMode = nullptr;
    GPS* m_gps = nullptr;
    QTimer* m_timer = nullptr;

    // 心跳
    int m_heartBeatInterval;
    int m_heartBeatCount = 0;
    QString m_heartBeatTopic;
    QJsonObject m_data2BackServerHeartbeat;

    int m_kafkaServerPort;
    int m_onbonServerPort;

    // 管控状态
    bool m_controlState = false;

    // 恢复默认节目时间
    int m_back2DefaultProgramTime;
    int m_back2DefaultProgramTimeCount = 0;

    // 触发状态
    e_triggerScreenPriority m_triggerScreenPriority
        = e_triggerScreenPriority::ALL;
};

#endif // MYMAIN_H
