#ifndef GPS_H
#define GPS_H

#include <QObject>
#include <QSerialPort>

typedef struct{
    QString BDGGA1;         // UTC时间：hhmmss.ss——000000.00~235959.99
    QString BDGGA2;         // 纬度，格式：ddmm.mmmm ——0000．00000~8959.9999 （标准的度分格式）
    QString BDGGA3;         // 南北半球——N北纬；S南纬
    QString BDGGA4;         // 经度格式：dddmm.mmmm ——00000.0000~17959.9999 （标准的度分格式）
    QString BDGGA5;         // U东西半球——E表示东经；W表示西经
    QString BDGGA6;         // 质量因子——0=未定位，1=GPS单点定位固定解，2=差分定位，3=PPS解；4=RTK固定解；5=RTK浮点解；6=估计值；7=手工输入模式；8=模拟模式；
    QString BDGGA7;         // 应用解算位置的卫星数——00~12
    QString BDGGA8;         // HDOP，水平图形强度因子——0.500~99.000 ；大于6不可用
    QString BDGGA9;         // 天线高程（海平面）——－9999.9～99999.9
    QString BDGGA10;        // 线线高程单位(m) ——m
    QString BDGGA11;        // 大地水准面起伏——地球椭球面相对大地水准面的高度
    QString BDGGA12;        // 大地水准面起伏单位(m)   ——m
    QString BDGGA13;        // 差分GPS数据期——差分时间（从最近一次接收到差分信号开始的秒数，如果不是差分定位将为空），不使用DGPS时为空
    QString BDGGA14;        // 基准站号——0000~1023；不使用DGPS时为空
}s_BDGGA;

class GPS : public QObject
{
    Q_OBJECT
public:
    explicit GPS(QJsonObject& cfgJson, QObject *parent = nullptr);
    void getGpsInfo(float& N, float& E);

private:
    void init(QJsonObject& cfgJson);
    bool getFirstData(QByteArray& data, QByteArray& first);
    void unPackBDGGA(QString BDGGA);

signals:
    void signalUpdateGpsNE(QString N, QString E);   // 更新gps信息

private slots:
    void slotRead();

private:
    QSerialPort* m_serialPort = nullptr;
    QByteArray m_dataTmp;
    QStringList m_BDGGA;
};

#endif // GPS_H
