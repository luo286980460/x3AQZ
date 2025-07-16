#ifndef I2CMANAGER_H
#define I2CMANAGER_H

#include <QObject>
#include <QThread>

class I2CManagerWorker;

class I2CManager : public QObject
{
    Q_OBJECT
public:
    explicit I2CManager(QJsonObject& cfgJson, QObject *parent = nullptr);
    ~I2CManager();

    void start();

signals:
    void signalInitWork();
    void signalUpdateI2CData(QByteArray data);

private:
    QThread m_workerThread;     // kafka 工作线程
    I2CManagerWorker *m_work;      // kafka 工作类
};

#endif // I2CMANAGER_H
