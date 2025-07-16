#ifndef SCREEN_H
#define SCREEN_H

#include <QObject>
#include <QSerialPort>
#include "bx_dual_sdk.h"

// 控制器类型
typedef enum{
    BX_5E1 = 0x0154,
    BX_5E2 = 0x0254,
    BX_5E3 = 0x0354,

    BX_6M0 = 0x0062,
    BX_6M1 = 0x0162,
    BX_6M2 = 0x0262,
    BX_6M3 = 0x0362,
    BX_6M = 0x0462,
    BX_6M0P = 0x0062,
    BX_6M1P = 0x0162,
    BX_6M2P = 0x4262,
    BX_6M3P = 0x4362,
    BX_6M4P = 0x4462,

    BX_6E1 = 0x0174,
    BX_6E2 = 0x0274,
    BX_6E3 = 0x0374,
    BX_6E1X = 0x0474,
    BX_6E2X = 0x0574,
    BX_6E1XP = 0x0674,
    BX_6E2XP = 0x0774,
    BX_6E3P = 0x0974,

    BX_6Q0 = 0x0066,
    BX_6Q1 = 0x0166,
    BX_6Q2 = 0x0266,
    BX_6Q2L = 0x0466,
    BX_6Q3 = 0x0366,
    BX_6Q3L = 0x0566,

    others5e = 0
}e_controllerType;

class Screen : public QObject
{
    Q_OBJECT
public:
    explicit Screen(QString portName, QObject *parent = nullptr);
    ~Screen();

private:
    void init();
    bool udpPing();
    void addController();
    QString getControllerType(ushort controllerType);

    // 静态区功能
    void sendStaticProgram(QString content, QString font, int fontSize);
    void addProgram_G6();
    void addArea_G6(Ouint16 AreaID,Ouint8 AreaType,Ouint16 AreaX,Ouint16 AreaY,Ouint16 AreaWidth,Ouint16 AreaHeight);
    void addAreaPicture_G6(Ouint16 AreaID, QString content, QString font, int fontSize);
    void comSendProgram_G5(Oint8* uartPort, Ouint8 baudRate);

    // test
    void aaddProgram_G6();
    void aaddArea_G6(Ouint16 AreaID,Ouint8 AreaType,Ouint16 AreaX,Ouint16 AreaY,Ouint16 AreaWidth,Ouint16 AreaHeight);
    void aaddAreaPicture_G6(int color, int areaId, QString fontName, int fontSize,
                            QString content, int Halign, int Valign, int DisplayMode,
                            int Speed);
    void com_send_program_G5(Oint8* uartPort, Ouint8 baudRate);

signals:

public slots:

    void slotTest(QByteArray jsonData);
    // serial 发送动态区
    void slotDynamicArea_AddAreaTxtDetails_6G_serial(int nBaudRateIndex, int color, int areaId, int areaX, int areaY,
                                                    int width, int height, QString fontName, int fontSize,
                                                    QString content, int Halign, int Valign, int DisplayMode,
                                                     int Speed);

    // 关闭动态区 区域 0xff 关闭所有
    void slotDynamicArea_DelArea_G5_Serial(int nBaudRateIndex, int areaId);

    // serial 发送静态区
    void slotStaticProgram(int nBaudRateIndex, int color, int areaId, int areaX, int areaY,
                            int width, int height, QString fontName, int fontSize,
                            QString content, int Halign, int Valign, int DisplayMode,
                            int Speed);

private:
    bool m_sdkInit = false;
    Ping_data* m_udpPingRetData = nullptr;      // udp ping回复
    QStringList m_controllerIpList;             // 控制器列表
    EQareaHeader_G6* oAreaHeader_G6 = nullptr;          // G6 动态区区域属性
    EQpageHeader_G6* stPageHeader = nullptr;            // G6 动态区页面属性
    QString m_portName;
};

#endif // SCREEN_H
