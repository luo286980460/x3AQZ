#include "i2cmanager.h"
#include "i2cmanagerworker.h"

#include <QJsonObject>

I2CManager::I2CManager(QJsonObject& cfgJson, QObject *parent)
    : QObject{parent}
{
    m_work = new I2CManagerWorker(cfgJson);
    m_work->moveToThread(&m_workerThread);
    connect(&m_workerThread, &QThread::finished, m_work, &QObject::deleteLater);

    connect(this, &I2CManager::signalInitWork, m_work, &I2CManagerWorker::init);
    connect(m_work, &I2CManagerWorker::signalUpdateI2CData, this, &I2CManager::signalUpdateI2CData);
    connect(m_work, &I2CManagerWorker::signalUpdateVolumeAndBrightness, this, &I2CManager::signalUpdateVolumeAndBrightness);
}

I2CManager::~I2CManager()
{
    m_workerThread.quit();
    m_workerThread.wait();
}

void I2CManager::start()
{
    m_workerThread.start();
    emit signalInitWork();
}
