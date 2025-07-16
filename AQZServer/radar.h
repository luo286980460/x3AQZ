#ifndef RADAR_H
#define RADAR_H

#include <QObject>
#include <QSerialPort>

struct s_ownProgram{                // 雷达自身激活的节目
    QString normalContent;          // 不超速显示的节目内容
    int normalContentColor;         // 不超速显示的节目颜色
    QString speedingContent;        // 超速显示的节目内容
    int speedingContenttColor;      // 超速显示的节目颜色
};

struct s_otherProgram{              // 雷达激活其他设备的节目
    QString content;                // 节目内容
    int color;                      // 颜色
    bool singleScreen;              // 单双屏
};

class Radar : public QObject
{
    Q_OBJECT
public:
    explicit Radar(QJsonObject& cfgJson, QObject *parent = nullptr);

private:
    void init(QJsonObject& cfgJson);


signals:
    void signalSendSpeedProgram2OnbonUp(QString speed, int color);
    void signalSendSpeedProgram2OnbonDown(QString content, int color);
    void signalUpdateDetectionModeProgram(QByteArray jsonData);    // 更新多端需要发送的节目的数据

private slots:
    void slotRead();

private:
    QSerialPort* m_serialPort = nullptr;
    int m_lastSpeed;                // 上一次的速度
    s_ownProgram m_ownProgram;      // 雷达激活自身的节目
    s_otherProgram m_otherProgram;  // 雷达激活其他端的节目
    int m_speedingThreshold;        // 超速阈值
    bool m_lastSpeedingState;       // 上次的超速状态
};

#endif // RADAR_H
