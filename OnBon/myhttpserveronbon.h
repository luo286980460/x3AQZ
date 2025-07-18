#ifndef MYHTTPSERVER_H
#define MYHTTPSERVER_H

#include <QObject>
#include <QDebug>
#include <QJsonObject>
#include "include/libhv/HttpServer.h"

using namespace hv;

class MyHttpServerOnBon : public QObject
{
    Q_OBJECT

public:
    MyHttpServerOnBon(int port, QJsonObject& cfgJson, QObject *parent = nullptr);
    virtual ~MyHttpServerOnBon();

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

    // ********** 屏幕控制 路由开始 **********

    // /screen/text2Dynamic
    bool missingParameterText2Dynamic(const QJsonObject& json,      // 是否缺少必要参数
                                    QJsonObject& backJson);
    QJsonObject parseScreenText2Dynamic(QJsonObject json,           // 数据解析
                                        QJsonObject& backJson);

    // /screen/closeDynamic
    bool missingParameterCloseDynamic(const QJsonObject& json,      // 是否缺少必要参数
                                      QJsonObject& backJson);
    QJsonObject parseScreenCloseDynamic(QJsonObject json,           // 数据解析
                                        QJsonObject& backJson);

    // /screen/text2Static
    QJsonObject parseScreenText2Static(QJsonObject json,           // 数据解析
                                        QJsonObject& backJson);

    // /screen/setBrightness
    QJsonObject parseScreenSetBrightness(QJsonObject json,           // 数据解析
                                         QJsonObject& backJson);

    // ********** 屏幕控制 路由结束 **********

signals:
    void signalTest(QByteArray jsonData);
    void signalWrite2Kafka(QString topic, QString strJson, QString strKey);
    void signalDynamicArea_AddAreaTxtDetails_6G_serial(int nBaudRateIndex, int color,
                                                         int areaId, int areaX, int areaY, int width,
                                                         int height, QString fontName, int fontSize,
                                                         QString content, int Halign, int Valign, int DisplayMode,
                                                         int Speed);
    void signalDynamicArea_DelArea_G5_Serial(int nBaudRateIndex, int areaId);
    void signalStaticProgram(int nBaudRateIndex, int color, int areaId, int areaX, int areaY,
                            int width, int height, QString fontName, int fontSize,
                            QString content, int Halign, int Valign, int DisplayMode,
                            int Speed);
    void signalSetLuminance(int brightnessI);

public slots:

private:
    hv::HttpServer* m_httpServer;
    HttpService* m_router;
    QByteArray m_aesKey = "Utis00000000LsCb";
};


#endif // MYHTTPSERVER_H
