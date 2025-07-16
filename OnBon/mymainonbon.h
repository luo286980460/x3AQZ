#ifndef MYMAINONBON_H
#define MYMAINONBON_H

#include <QObject>

class MyHttpServerOnBon;
class Screen;

class MyMainOnBon : public QObject
{
    Q_OBJECT
public:
    explicit MyMainOnBon(QObject *parent = nullptr);

private:
    bool loadCfg(QJsonObject& cfgJson);
    void initHttpServer(QJsonObject& cfgJson);
    void initScreen(QJsonObject& cfgJson);

signals:
    void signalWrite2Kafka(QString strJson, QString strKey);

public slots:


private:
    MyHttpServerOnBon* m_httpserver = nullptr;
    Screen* m_screen = nullptr;
};

#endif // MYMAINONBON_H
