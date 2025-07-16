#include <QSettings>
#include <QCoreApplication>
#include <QFileInfo>

#include "producer.h"
#include "producerworker.h"

Producer::Producer(QString ipPort, QString topic, QObject *parent)
    : QObject{parent}
    , m_topic(topic)
{

    m_work = new producerWorker(ipPort.toStdString(), topic.toStdString());
    m_work->moveToThread(&m_workerThread);
    connect(&m_workerThread, &QThread::finished, m_work, &QObject::deleteLater);
    connect(this, &Producer::signalInitWork, m_work, &producerWorker::init);
    connect(this, &Producer::signalProducerMsgJson, m_work, &producerWorker::slotProducerMsgJson);
}

Producer::~Producer()
{
    m_workerThread.quit();
    m_workerThread.wait();
}

void Producer::start()
{
    m_workerThread.start();
    emit signalInitWork();
}

void Producer::stop()
{
    m_workerThread.quit();
    m_workerThread.wait();
}

QString Producer::getTopic()
{
    return m_topic;
}
