#ifndef MYHTTPSERVER_H
#define MYHTTPSERVER_H

#include <QObject>
#include <QDebug>
#include <QJsonObject>
#include "include/libhv/HttpServer.h"

using namespace hv;

class Controller;
// 单屏4字 双屏4字
// 单屏6字 双屏6字

// 字体 文字数量 颜色(根据字体 字数 计算字号)

// 四个字满屏 动态区 居中静止显示 宋体 48 交通46
// 6字单屏 动态区 居中静止显示 宋体31 交通30
// 8字双屏 静态区 快速打印 宋体48
// 8字单屏 动态区 居中静止显示 宋体 交通24
// 16字单屏 动态区 居中静止显示 宋体
// 16字双屏  静态区 快速打印 宋体

// 上速度 64*64 字号3位数30号字体 2位数42号字体 等线
// 下提醒4字 36号  速度管理 不超速绿 10%黄色 10%以上红色


// 交通字体 文字 34size 数字40size2位数   数字30 size3位数


// 默认节目: 8字屏幕切换 快速打印 (节目内容可能配置)
// 雷达检测: 速度加提醒  (回默认节目的时间)
// 后台管控: ...    (T0 其他节目全部禁止)
//

// 车端激活 自己显示速度 行人端显示有车
// 行人端过人 车端提醒行人通过  行人端提示警示
// 冲突的时候 对方信息优先

// 单屏2(插入空格发) 4 5 6 7 8 12双排 16双排
// 双屏4 8 10 12 14

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
    QJsonObject parseScreenOpenControl(const QJsonObject& json,     // 解析 /screen/openControl
                                           QJsonObject& backJson);

signals:
    void signalWrite2Kafka(QString topic, QString strJson, QString strKey);
    void signalOpenControl(QByteArray jsonData, bool open);

public slots:

public:
    hv::HttpServer* m_httpServer = nullptr;
    HttpService* m_router = nullptr;

private:
    QByteArray m_aesKey = "Utis00000000LsCb";
};


#endif // MYHTTPSERVER_H
