#include "i2cmanagerworker.h"
#include "qdebug.h"

#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define I2C_DEV "/dev/i2c-3"

//  BH1750
#define BH1750_DEV_ADDR 0x23
#define BH1750_MODE 0x10        // 连续高精度模式
#define BH1750_POWER_ON 0x01    // 通电命令
#define BH1750_RESET 0x07       // 复位命令

//  ina226
#define INA226_ADDR 0x40  // 默认I2C地址
#define CAL_REG 0x05      // 校准寄存器
#define SET_REG 0x00      // 配置寄存器
#define SHUNT_V_REG 0x01  // 分流电压寄存器
#define CURRENT_REG 0x04  // 电流寄存器
#define VBUS_REG 0x02     // 读取电压寄存器（0x02）
#define VSHUNT_REG 0x01   // 读取电压寄存器（0x01）
#define POWER_REG 0x03    // 读取功率寄存器（0x01）


I2CManagerWorker::I2CManagerWorker(int interval, QObject *parent)
    : QObject{parent}
    , m_interval(interval)
{

}

float I2CManagerWorker::readLux()
{
    uint8_t buf[2];
    uint8_t cmd = BH1750_MODE;

    if (ioctl(m_fd, I2C_SLAVE, BH1750_DEV_ADDR) < 0) {
        qDebug() << "设置从机地址失败";
        return -1;
    }

    if (write(m_fd, &cmd, 1) != 1) {
        qDebug() << "发送命令失败";
        return -2;
    }

    usleep(120000);  // 等待测量完成（BH1750需120ms）

    if (read(m_fd, buf, 2) != 2) {
        qDebug() << "读取失败";
        return -3;
    }

    return (buf[0] << 8 | buf[1]) / 1.2;  // 计算Lux值
}

void I2CManagerWorker::initTimer()
{
    m_timer = new QTimer;
    m_timer->setInterval(m_interval);
    connect(m_timer, &QTimer::timeout, this, [this]{
        // 电压    0x40 /dev/i2c-3
        // 电流 注意: 电流<=0.1A       结果: 电压+0.3v
        // 电流>0.1A && 电流<8A            结果: 电压+(电流A * 0.04 + 0.3)
        // 电流>8A                        结果: 电压+0.62v

        // readINA226();
        float current = get_current();
        float busVoltage = read_bus_voltage();
        float busVoltage1 = busVoltage;
        if(current <= 0.1){
            busVoltage1 += 0.3;
        }else if(current > 0.1 && current < 8){
            busVoltage1 += (current * 0.04) + 0.3;
        }else if(current > 8){
            busVoltage1 += 0.62;
        }
        float shuntVoltage = read_shunt_voltage()*1000;
        float power = read_power();
        float Lux = readLux();

        // qDebug() << "***********************";
        // qDebug() << QString("* 总线电压: %1").arg(busVoltage, 0, 'f', 4) << "*";
        // qDebug() << QString("* 总线电压: %1").arg(busVoltage1, 0, 'f', 4) << "*";
        // qDebug() << QString("* 分流电压: %1").arg(shuntVoltage, 0, 'f', 4) << " *";
        // qDebug() << QString("* 电    流: %1").arg(current, 0, 'f', 4) << " *";
        // qDebug() << QString("* 功    率: %1").arg(power, 0, 'f', 4) << " *";
        // QThread::msleep(100);
        // qDebug() << QString("* 光线强度: %1").arg(Lux, 0, 'f', 4) << "*";
        // qDebug() << "***********************";

        // qDebug() << "\n";

        initIna226Config();
        QJsonObject json;
        json.insert("busVoltage", QString("%1").arg(busVoltage1, 0, 'f', 4));
        json.insert("shuntVoltage", QString("%1").arg(shuntVoltage, 0, 'f', 4));
        json.insert("current", QString("%1").arg(current, 0, 'f', 4));
        json.insert("power", QString("%1").arg(power, 0, 'f', 4));
        json.insert("Lux", QString("%1").arg(Lux, 0, 'f', 4));
        emit signalUpdateI2CData(QJsonDocument(json).toJson());
        //qDebug() << json;
    });
    m_timer->start();
}

void I2CManagerWorker::initIna226Config()
{
    // 设置从机地址
    if (ioctl(m_fd, I2C_SLAVE, INA226_ADDR) < 0) {
        qDebug() << "设置从机地址失败";
        return;
    }

    // 写入校准
    m_ina226Info.shunt_r = 0.01;
    m_ina226Info.max_current = 8;
    m_ina226Info.current_lsb = m_ina226Info.max_current / 32768.0f;

    uint16_t cal_value = 0.00512 / (m_ina226Info.current_lsb * m_ina226Info.shunt_r); // 校准公式
    write_register(CAL_REG, cal_value);

    // 写入配置
    const uint16_t config = 0x4527;
    write_register(SET_REG, config);
}

void I2CManagerWorker::write_register(uint8_t reg, uint16_t value)
{
    uint8_t buf[3];
    buf[0] = reg;
    buf[1] = (value >> 8) & 0xFF;
    buf[2] = value & 0xFF;
    write(m_fd, buf, 3);
}

uint16_t I2CManagerWorker::read_register(uint8_t reg)
{
    uint8_t buf[2];
    write(m_fd, &reg, 1);
    QThread::msleep(50);
    read(m_fd, buf, 2);
    return (buf[0] << 8) | buf[1];
}

float I2CManagerWorker::get_current()
{
    int16_t raw_current = read_register(CURRENT_REG);
    return (raw_current * m_ina226Info.current_lsb);  // 直接返回 A
}

float I2CManagerWorker::read_bus_voltage()
{
    uint16_t raw = read_register(VBUS_REG);  // 读取电压寄存器（0x02）
    return raw * 0.00125f;  // LSB=1.25mV，转换为V
}

float I2CManagerWorker::read_shunt_voltage()
{
    int16_t raw = (int16_t)read_register(VSHUNT_REG);  // 分流电压寄存器（0x01）
    return raw * 0.0000025;  // LSB=2.5μV，转换为V
}

float I2CManagerWorker::read_power()
{
    uint16_t raw_power = read_register(POWER_REG);  // 功率寄存器（0x03）
    return raw_power * 25 * m_ina226Info.current_lsb;  // LSB=25mW，转换为W
}

void I2CManagerWorker::init()
{
    m_fd = open("/dev/i2c-3", O_RDWR);
    if (m_fd < 0) {
        qDebug() << "/dev/i2c-3: 打开失败";
        return;
    }
    qDebug() << "/dev/i2c-3: 打开成功";

    initIna226Config();
    initTimer();

}
