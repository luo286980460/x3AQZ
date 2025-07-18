#include <QJsonObject>

#include "gps.h"

GPS::GPS(QJsonObject& cfgJson, QObject *parent)
    : QObject{parent}
{
    init(cfgJson);
}

void GPS::getGpsInfo(float &N, float &E)
{
    N = m_BDGGA.at(1).toFloat();
    E = m_BDGGA.at(3).toFloat();
}

void GPS::init(QJsonObject &cfgJson)
{
    m_serialPort = new QSerialPort(this);
    connect(m_serialPort, &QSerialPort::readyRead, this, &GPS::slotRead);
    m_serialPort->setPortName(cfgJson.value("portName").toString());
    m_serialPort->setBaudRate(cfgJson.value("baudRate").toInt());

    if(!m_serialPort->open(QIODevice::ReadWrite)){
        qDebug() << m_serialPort->portName() << "[gps] : 打开失败";
        return;
    }

    qDebug() << m_serialPort->portName() << "[gps] : 打开成功";
}

void GPS::slotRead()
{
    m_dataTmp += m_serialPort->readAll();
    QByteArray ba1;

    while(getFirstData(m_dataTmp, ba1)){
        ba1 = ba1.trimmed();
        if(QString(ba1).contains("$BDGGA")){
            unPackBDGGA(QString(ba1));
            // qDebug() << "m_BDGGA:" << m_BDGGA;
        }
        ba1.clear();
    }
}


bool GPS::getFirstData(QByteArray &data, QByteArray &first)
{
    // 去除串头可能出现的其他字符
    while(data.size() > 0 && data[0] != '$'){
        data.remove(0, 1);
    }

    for(int i=0; i<data.size(); i++){

        if(data.at(i) == '\r'){
            data.remove(0, i+1);
            return true;
        }

        first.append(data.at(i));
    }
    return false;
}



void GPS::unPackBDGGA(QString BDGGA)
{
    s_BDGGA bdgga;
    m_BDGGA = BDGGA.split(",");
    if(m_BDGGA.size() != 15){
        return;
    }

    m_BDGGA.removeFirst();
}

