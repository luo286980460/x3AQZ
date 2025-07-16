#include <QJsonObject>
#include <QJsonDocument>

#include "radar.h"
#include "mymain.h"

Radar::Radar(QJsonObject& cfgJson, QObject *parent)
    : QObject{parent}
{
    init(cfgJson);
}

void Radar::init(QJsonObject &cfgJson)
{
    m_serialPort = new QSerialPort;
    connect(m_serialPort, &QSerialPort::readyRead, this, &Radar::slotRead);
    m_serialPort->setPortName(cfgJson.value("portName").toString());
    m_serialPort->setBaudRate(cfgJson.value("baudRate").toInt());

    if(!m_serialPort->open(QIODevice::ReadWrite)){
        qDebug() << m_serialPort->portName() << "[雷达] : 打开失败";
        return;
    }

    qDebug() << m_serialPort->portName() << "[雷达] : 打开成功";

    m_speedingThreshold = cfgJson.value("speedingThreshold").toInt();

    QJsonObject ownProgramJson = cfgJson.value("ownProgram").toObject();
    m_ownProgram.normalContent = ownProgramJson.value("normalContentColor").toString();
    m_ownProgram.normalContentColor = ownProgramJson.value("normalContent").toInt();
    m_ownProgram.speedingContent = ownProgramJson.value("speedingContent").toString();
    m_ownProgram.speedingContenttColor = ownProgramJson.value("speedingContenttColor").toInt();


    QJsonObject otherProgramJson = cfgJson.value("otherProgramJson").toObject();
    m_otherProgram.content = otherProgramJson.value("content").toString(); // 多端激活时发送的内容
    m_otherProgram.color = otherProgramJson.value("color").toBool(); // 多端激活时发送的单双屏
    m_otherProgram.singleScreen = otherProgramJson.value("singleScreen").toInt();      // 多端激活时发送的颜色
}

void Radar::slotRead()
{
    QJsonObject dataJson;
    QString data = m_serialPort->readAll();
    QString splitStr;

    if(data.isEmpty()){
        qDebug() << "雷达数据为空";
        return;
    }

    dataJson.insert("方向", "未知");
    if(data.contains("+")){
        dataJson["方向"] = "来";
        splitStr = "+";
    }else if(data.contains("-")){
        dataJson.insert("方向", "去");
        splitStr = "-";
    }else{
        qDebug() << "雷达数据格式有误, 没有 + - ";
        return;
    }

    QStringList dataList = data.split(splitStr, Qt::SkipEmptyParts);
    if(dataList.size() != 2){
        qDebug() << "雷达数据格式有误, + - 分割完数量不为2";
        return;
    }


    int currentSpeed = (int)dataList.at(1).toFloat();
    bool speedingState = currentSpeed > m_speedingThreshold ? true : false;

    if(m_lastSpeed == currentSpeed){
        return;
    }
    m_lastSpeed = currentSpeed;
    dataJson.insert("speed", m_lastSpeed);

    if(speedingState != m_lastSpeedingState){
        signalSendSpeedProgram2OnbonDown(speedingState ? m_ownProgram.speedingContent : m_ownProgram.normalContent,
                                         speedingState ? m_ownProgram.speedingContenttColor : m_ownProgram.normalContentColor);
    }

    emit signalSendSpeedProgram2OnbonUp(QString::number(m_lastSpeed), speedingState ? 1 : 2);

    // 给其他端发送节目
    QJsonObject json;
    json.insert("content", m_otherProgram.content);
    json.insert("signalScreen", m_otherProgram.singleScreen);
    json.insert("color", m_otherProgram.color);

    emit signalUpdateDetectionModeProgram(QJsonDocument(json).toJson());    // 更新多端需要发送的节目的数据

    qDebug() << m_lastSpeed;
}
