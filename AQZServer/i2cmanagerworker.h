#ifndef I2CMANAGERWORKER_H
#define I2CMANAGERWORKER_H

#include <QObject>
#include <QTimer>

struct s_ina226{
    float shunt_r;      // 分流电阻（Ω）
    float max_current;  // 最大电流（A）
    float current_lsb;  // 61.035 µA/bit
};

class I2CManagerWorker : public QObject
{
    Q_OBJECT
public:
    explicit I2CManagerWorker(int interval, QObject *parent = nullptr);

private:
    float readLux();
    void initTimer();

    // ina226
    // 初始化信息
    void initIna226Config();
    void write_register(uint8_t reg, uint16_t value);   // 写入寄存器
    uint16_t read_register(uint8_t reg);    // 读取寄存器
    float get_current();                    // 获取电流值（mA）
    float read_bus_voltage();               // 读取总线电压（Vbus，单位：V）
    float read_shunt_voltage();             // 读取分流电压（Vshunt，单位：V）
    float read_power();                     // 读取当前功率（单位：W）

public slots:
    void init();

signals:
    void signalUpdateI2CData(QByteArray data);

private:
    int m_fd;
    QTimer* m_timer = nullptr;
    int m_interval;
    s_ina226 m_ina226Info;
};

#endif // I2CMANAGERWORKER_H
