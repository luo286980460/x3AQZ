#ifndef MYHTTPSERVERKAFKA_H
#define MYHTTPSERVERKAFKA_H

#include <QObject>
#include <QDebug>
#include <QJsonObject>
#include "include/libhv/HttpServer.h"

class Producer;

using namespace hv;

class MyHttpServerKafka : public QObject
{
    Q_OBJECT

public:
    MyHttpServerKafka(int port, QJsonObject& cfgJson, QObject *parent = nullptr);
    virtual ~MyHttpServerKafka();

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

    // ****** kafka ******
    void initProducer(QJsonObject& cfgJson);                    // 初始化
    Producer* getProducerFromTopic(const QString& topic);       // 以主题获取创造者
    bool missingParameterProducer(const QJsonObject& json,      // producer 是否缺少必要参数
                                  QJsonObject& backJson);
    QJsonObject parseLightProducer(const QJsonObject& json,     // 解析 producer
                                   QJsonObject& backJson);
    // ****** kafka ******

signals:
    void signalWrite2Kafka(QString topic, QString strJson, QString strKey);

public slots:

private:
    hv::HttpServer* m_httpServer;
    HttpService* m_router;
    QList<Producer*> m_producerList;    // 创造者列表

private:
    QByteArray m_aesKey = "Utis00000000LsCb";
};


#endif // MYHTTPSERVERKAFKA_H
