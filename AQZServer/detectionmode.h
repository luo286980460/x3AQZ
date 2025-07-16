#ifndef DETECTIONMODE_H
#define DETECTIONMODE_H

// 检测模式

#include "qjsonobject.h"
#include <QObject>
#include <QSerialPort>
#include <QTimer>

enum class e_detectionMode{ // 检测模式
    MASTER,                 // 主机
    SLAVE,                  // 从机
    ALL                     // 不做多端
};

class DetectionMode : public QObject
{
    Q_OBJECT
public:
    explicit DetectionMode(QJsonObject& cfgJson, QObject *parent = nullptr);

private:
    void init(QJsonObject& cfgJson);
    void initTimer(QJsonObject& cfgJson);
    void writeData();

signals:
    void signalPlayOtherLuaProgram(QByteArray jsonData);

public slots:
    void slotRead();
    void slotUpdateDetectionModeProgram(QByteArray jsonData);    // 更新多端需要发送的节目的数据

private:
    QSerialPort* m_serialPort = nullptr;
    QTimer* m_timer = nullptr;
    QJsonObject m_jsonData;
    bool m_luraConnected = false;
    e_detectionMode m_detectionMode;
    QJsonObject m_detectionModeProgram; // 多端节目信息
};

#endif // DETECTIONMODE_H
