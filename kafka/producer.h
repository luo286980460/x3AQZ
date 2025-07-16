#ifndef PRODUCER_H
#define PRODUCER_H

#include <QObject>
#include <QThread>

class producerWorker;

class Producer : public QObject
{
    Q_OBJECT
public:
    explicit Producer(QString ipPort, QString topic, QObject *parent = nullptr);
    ~Producer();
    void start();
    void stop();

    QString getTopic();

signals:
    void showMsg(QString);
    void signalInitWork();
    void signalProducerMsgJson(QString strJson, QString strKey);

public slots:

private:
    QThread m_workerThread;     // kafka 工作线程
    producerWorker *m_work      // kafka 工作类
        = nullptr;
    QString m_topic;            // 主题
};

#endif // PRODUCER_H
