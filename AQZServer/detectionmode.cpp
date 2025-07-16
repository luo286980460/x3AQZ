#include <QJsonDocument>

#include "detectionmode.h"

DetectionMode::DetectionMode(QJsonObject& cfgJson, QObject *parent)
    : QObject{parent}
{
    init(cfgJson);

    if(m_detectionMode == e_detectionMode::MASTER){
        initTimer(cfgJson);
    }
}

void DetectionMode::init(QJsonObject &cfgJson)
{
    m_serialPort = new QSerialPort;
    connect(m_serialPort, &QSerialPort::readyRead, this, &DetectionMode::slotRead);
    m_serialPort->setPortName(cfgJson.value("portName").toString());
    m_serialPort->setBaudRate(cfgJson.value("baudRate").toInt());

    if(!m_serialPort->open(QIODevice::ReadWrite)){
        qDebug() << m_serialPort->portName() << "[lura] : 打开失败";
        return;
    }

    qDebug() << m_serialPort->portName() << "[lura] : 打开成功";

    // 属性获取和保存
    m_detectionMode = (e_detectionMode)(cfgJson.value("detectionMode").toInt());
    if(m_detectionMode == e_detectionMode::MASTER){
        m_jsonData.insert("reader", (int)e_detectionMode::SLAVE);
    }else{
        m_jsonData.insert("reader", (int)e_detectionMode::MASTER);
    }
    qDebug() << m_jsonData;
}

void DetectionMode::initTimer(QJsonObject& cfgJson)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(cfgJson.value("loopTimer").toInt());
    connect(m_timer, &QTimer::timeout, this, [this]{
        if(!m_luraConnected && m_serialPort && m_serialPort->isOpen()){
            writeData();
        }
        m_luraConnected = false;
    });
    m_timer->start();
}

void DetectionMode::writeData()
{
    m_jsonData["data"] = m_detectionModeProgram;
    m_serialPort->write(QJsonDocument(m_jsonData).toJson());
    m_detectionModeProgram = {};
}

void DetectionMode::slotRead()
{
    QJsonObject json = QJsonDocument::fromJson(m_serialPort->readAll()).object();
    e_detectionMode detectionMode = (e_detectionMode)(json.value("reader").toInt());
    if(m_detectionMode != detectionMode){
        return;
    }

    // 有节目发节目
    QJsonObject data = json.value("data").toObject();
    if(!data.isEmpty()){
        emit signalPlayOtherLuaProgram(QJsonDocument(json.value("data").toObject()).toJson());
    }

    m_luraConnected = true;
    writeData();
}

void DetectionMode::slotUpdateDetectionModeProgram(QByteArray jsonData)
{
    m_detectionModeProgram = QJsonDocument::fromJson(jsonData).object();
}
