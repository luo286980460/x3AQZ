#ifndef I2CMANAGERWORKER_H
#define I2CMANAGERWORKER_H

#include <QObject>
#include <QTimer>
#include <QList>

struct s_ina226{
    float shunt_r;      // 分流电阻（Ω）
    float max_current;  // 最大电流（A）
    float current_lsb;  // 61.035 µA/bit
};

struct s_LuxLevel{      // 光照等级对应的亮度和音量
    int miniLux;
    int maxLux;
    int screenBrightness;
    int volume;
};

class I2CManagerWorker : public QObject
{
    Q_OBJECT
public:
    explicit I2CManagerWorker(QJsonObject& cfgJoson, QObject *parent = nullptr);

private:
    float readLux();
    void initTimer();

    // ina226
    // 初始化信息
    void initIna226Config();
    void write_register(uint8_t reg, uint16_t value);   // 写入寄存器
    uint16_t read_register(uint8_t reg);                // 读取寄存器
    float get_current();                                // 获取电流值（mA）
    float read_bus_voltage();                           // 读取总线电压（Vbus，单位：V）
    float read_shunt_voltage();                         // 读取分流电压（Vshunt，单位：V）
    float read_power();                                 // 读取当前功率（单位：W）
    bool getVolumeAndBrightnessFromLux(int Lux);        // 从光照等级获取音量和亮度

public slots:
    void init();

signals:
    void signalUpdateI2CData(QByteArray data);          // 更新外部i2c数据
    void signalUpdateVolumeAndBrightness(int volume,    // 更新外部音量和亮度数据
                                         int screenBrightness);

private:
    int m_fd;
    QTimer* m_timer = nullptr;
    int m_interval;
    s_ina226 m_ina226Info;
    QList<s_LuxLevel> m_luxLevelList;
    int m_volume = 10;
    int m_screenBrightness = 15;
    QList<int> m_luxList;
    int m_times;    // 光照强度的最大数量
};

#endif // I2CMANAGERWORKER_H
