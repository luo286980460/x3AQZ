#ifndef MYHTTPSERVER_H
#define MYHTTPSERVER_H

#include <QObject>
#include <QDebug>
#include <QJsonObject>
#include "include/libhv/HttpServer.h"

using namespace hv;

class Controller;

// 车端激活 自己显示速度 行人端显示有车
// 行人端过人 车端提醒行人通过  行人端提示警示
// 冲突的时候 对方信息优先

// /dev/ttyS5           屏幕
// /dev/ttyCH343USB0    lura
// /dev/ttyCH343USB1    radar
// /dev/ttyCH343USB2    weather
// /dev/ttyCH343USB3    gnss
// 光敏    0x23 /dev/i2c-3
// 电压    0x40 /dev/i2c-3
    // 电流 功率 注意: 电流<=0.1A       结果: 电压+0.3v
    // 电流>0.1A && 电流<8A            结果: 电压+(电流A * 0.04 + 0.3)
    // 电流>8A                        结果: 电压+0.62v


/*
    雷达触发节目 开关 启动时开不开
    默认节目信息放配置文件
*/

class MyHttpServer : public QObject
{
    Q_OBJECT

public:
    MyHttpServer(QJsonObject& cfgJson, QObject *parent = nullptr);
    virtual ~MyHttpServer();

    void stop();

private:
    void createHttpserver(int port);                // 开启httpserver
    bool ipAddrIsOK(const QString & ip);            // 判断字符串是否为合法 ip 地址
    QString qstr2Hex(QString instr);                // 汉字 转 16进制

    void add_file_handler(HttpServer& server, const QString& basepath, const QString& path);
    void add_directory_handlers(HttpServer& server, const QString& basepath, const QString& path = "");
    QByteArray crypt_Aes128_ECB_PKCS7_HEX(QByteArray plaintext, QByteArray key);
    QByteArray decrypt_Aes128_ECB_PKCS7_HEX(QByteArray plaintext, QByteArray key);

    bool headerIsOk(HttpRequest* req, QJsonObject& backJson);
    int respReturnJson(HttpResponse* resp, QJsonObject json);

    // /screen/openControl
    bool missingScreenOpenControl(const QJsonObject& json,      // /screen/openControl 是否缺少必要参数
                                  QJsonObject& backJson);
public:
    QJsonObject parseScreenOpenControl(const QJsonObject& json, // 解析 /screen/openControl
                                           QJsonObject& backJson);

signals:
    void signalWrite2Kafka(QString topic, QString strJson, QString strKey);
    void signalOpenControl(QByteArray jsonData, bool open);
    void signalSetDefaultProgam(QByteArray jsonData);

public slots:

public:
    hv::HttpServer* m_httpServer = nullptr;
    HttpService* m_router = nullptr;

private:
    QByteArray m_aesKey = "Utis00000000LsCb";
    int m_volume = 10;
    int m_screenBrightness = 15;
};


#endif // MYHTTPSERVER_H
