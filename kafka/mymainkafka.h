#ifndef MYMAINKAFKA_H
#define MYMAINKAFKA_H

#include <QObject>

class MyHttpServerKafka;
class Producer;

class MyMainKafka : public QObject
{
    Q_OBJECT
public:
    explicit MyMainKafka(QObject *parent = nullptr);

private:
    bool loadCfg(QJsonObject& cfgJson);
    void initHttpServer(QJsonObject& cfgJson);


signals:
    void signalWrite2Kafka(QString strJson, QString strKey);

public slots:


private:
    MyHttpServerKafka* m_httpserver;
};

#endif // MYMAINKAFKA_H
