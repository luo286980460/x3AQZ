#include "screen.h"
#include "qjsondocument.h"
#include "qjsonobject.h"

#include <QDebug>
#include <QSerialPortInfo>
#include <QThread>

Screen::Screen(QString portName, QObject *parent)
    : QObject{parent}
    , m_portName(portName)
{
    qDebug() << "m_portName:  " << m_portName;
    init();
}

Screen::~Screen()
{
    #ifdef Q_OS_WIN
        bxDual_ReleaseSdk();
    #endif
}

void Screen::init()
{
    int res;
    #ifdef Q_OS_WIN
        if(bxDual_InitSdk() == 0){
            m_sdkInit = true;
            qDebug() << ("SDK 初始化成功");
        }else{
            m_sdkInit = false;
            qDebug() << ("SDK 初始化失败");
        }
    #elif defined(Q_OS_LINUX)
        m_sdkInit = true;
    #else
        m_sdkInit = false;
    #error "Unsupported platform!"
    #endif

    if(m_sdkInit){
        m_udpPingRetData = new Ping_data;
        memset(m_udpPingRetData, 0, sizeof(Ping_data));
        oAreaHeader_G6 = new EQareaHeader_G6;
        stPageHeader = new EQpageHeader_G6;
        memset(oAreaHeader_G6, 0, sizeof(EQareaHeader_G6));
        memset(stPageHeader, 0, sizeof(EQpageHeader_G6));

        bxDual_program_setScreenParams_G56(eSCREEN_COLOR_DOUBLE, 0, eDOUBLE_COLOR_PIXTYPE_1);

        oAreaHeader_G6->AreaType = 0x10;		//区域类型；动态区时，固定为0x10;
        oAreaHeader_G6->AreaX = 0;              //区域左上角横坐标
        oAreaHeader_G6->AreaY = 0;              //区域左上角纵坐标
        oAreaHeader_G6->AreaWidth = 32;         //区域宽度
        oAreaHeader_G6->AreaHeight = 32;		//区域高度
        oAreaHeader_G6->BackGroundFlag = 0x00;  //是否有背景
        oAreaHeader_G6->Transparency = 101;     //透明度
        oAreaHeader_G6->AreaEqual = 0x00;       //前景、背景区域大小是否相同

        stPageHeader->PageStyle = 0x00;			//数据页类型
        stPageHeader->DisplayMode = 0x02;		//显示方式:0x00 –随机显示; 0x01–静止显示; 0x02–快速打出; 0x03–向左移动; 0x04 –向左连移; ... 0x25 –向右移动  0x26 –向右连移  0x27 –向下移动  0x28 –向下连移
        stPageHeader->ClearMode = 0x00;			//退出方式/清屏方式
        stPageHeader->Speed = 15;				//速度等级
        stPageHeader->StayTime = 100;           //停留时间
        stPageHeader->RepeatTime = 1;           //重复次数
        stPageHeader->ValidLen = 10;			//此字段只在左移右移方式下有效
        stPageHeader->CartoonFrameRate = 0x00;  //特技为动画方式时，该值代表其帧率
        stPageHeader->BackNotValidFlag = 0x00;  //背景无效标志
        //字体信息-------------------------------------------------------------------------------------------------------
        stPageHeader->arrMode = E_arrMode::eMULTILINE;			//排列方式--单行多行
        stPageHeader->fontSize = 15;			//字体大小
        stPageHeader->color = eRED;             //字体颜色 E_Color_G56此通过此枚举值可以直接配置七彩色，如果大于枚举范围使用RGB888模式
        stPageHeader->fontBold = 0;             //是否为粗体
        stPageHeader->fontItalic = 0;           //是否为斜体
        stPageHeader->tdirection = E_txtDirection::pNORMAL;	//文字方向
        stPageHeader->txtSpace = 10;            //文字间隔
        stPageHeader->Halign = 2;               //横向对齐方式（0系统自适应、1左对齐、2居中、3右对齐）
        stPageHeader->Valign = 2;               //纵向对齐方式（0系统自适应、1上对齐、2居中、3下对齐）
    }
}

bool Screen::udpPing()
{
    QString ip;
    int errorNum = bxDual_cmd_udpPing(m_udpPingRetData);
    // 查询失败
    if(errorNum != 0) return false;
    // 获取查询到的 ip
    for(int i=0; i<20; i++){
        char c = (char)(m_udpPingRetData->ipAdder[i]);
        if(c != 0)
            ip.append((char)(m_udpPingRetData->ipAdder[i]));
    }

    // 确认 ip 是否已经查询过
    bool exs = false;
    foreach(QString controllerIp, m_controllerIpList ){
        if(controllerIp == ip + "*" + getControllerType(m_udpPingRetData->ControllerType)) {
            exs = true;
            break;
        }
    }

    // ip 已查询过 查询失败
    if(exs) return false;
    // ip 未查询过 查询成功
    return true;
}

void Screen::addController()
{
    QString controllerIp;

    // 获取查询到的 ip
    for(int i=0; i<20; i++){
        char c = (char)(m_udpPingRetData->ipAdder[i]);
        if(c != 0)
            controllerIp.append((char)(m_udpPingRetData->ipAdder[i]));
    }

    controllerIp += "*";
    controllerIp += getControllerType(m_udpPingRetData->ControllerType);
    m_controllerIpList << controllerIp;
}

QString Screen::getControllerType(ushort controllerType)
{
    switch(controllerType){
    case BX_5E1:
        return "BX_5E1";
        break;
    case BX_5E2:
        return "BX_5E2";
        break;
    case BX_5E3:
        return "BX_5E3";
        break;
    case BX_6M0:
        return "BX_6M0";
        break;
    case BX_6M1:
        return "BX_6M1";
        break;
    case BX_6M2:
        return "BX_6M2";
        break;
    case BX_6M3:
        return "BX_6M3";
        break;
    case BX_6M:
        return "BX_6M";
        break;
    case BX_6M2P:
        return "BX_6M2P";
        break;
    case BX_6M3P:
        return "BX_6M3P";
        break;
    case BX_6M4P:
        return "BX_6M4P";
        break;
    case BX_6E1:
        return "BX_6E1";
        break;
    case BX_6E2:
        return "BX_6E2";
        break;
    case BX_6E3:
        return "BX_6E3";
        break;
    case BX_6E1X:
        return "BX_6E1X";
        break;
    case BX_6E2X:
        return "BX_6E2X";
        break;
    case BX_6E1XP:
        return "BX_6E1XP";
        break;
    case BX_6E2XP:
        return "BX_6E2XP";
        break;
    case BX_6E3P:
        return "BX_6E3P";
        break;
    case BX_6Q0:
        return "BX_6Q0";
        break;
    case BX_6Q1:
        return "BX_6Q1";
        break;
    case BX_6Q2:
        return "BX_6Q2";
        break;
    case BX_6Q2L:
        return "BX_6Q2L";
        break;
    case BX_6Q3:
        return "BX_6Q3";
        break;
    case BX_6Q3L:
        return "BX_6Q3L";
        break;
    default:
        return "others";
        break;
    }
}

void Screen::sendStaticProgram(QString content, QString font, int fontSize)
{

    // aaddProgram_G6();
    // aaddArea_G6(0,0,0,0,64,256);
    // aaddAreaPicture_G6(0,(Ouint8*)content.toLocal8Bit().data(), 0x02, 1);
    // com_send_program_G5(m_portName.toLocal8Bit().data(), 2);

    // addProgram_G6();
    // addArea_G6(0,0,0,0,64,256);
    // addAreaPicture_G6(0, content, font, fontSize);
    // comSendProgram_G5(m_portName.toLocal8Bit().data(), 2);

    // EQprogramHeader_G6 programH;
    // Ouint16 areaID = 0;
    // EQareaHeader_G6 aheader;
    // BXSound_6G sound_6G;
    // EQpageHeader_G6 pheader;
    // EQprogram_G6 program;

    // memset(&programH, 0, sizeof(EQprogramHeader_G6));
    // memset(&aheader, 0, sizeof(EQareaHeader_G6));
    // memset(&sound_6G, 0, sizeof(BXSound_6G));
    // memset(&pheader, 0, sizeof(EQpageHeader_G6));
    // memset(&program, 0, sizeof(EQprogram_G6));

    // // programH
    // programH.FileType = 0x00;
    // programH.ProgramID = 0x01;
    // programH.ProgramStyle = 0x01;
    // programH.ProgramPriority = 0x01;

    // // aheader
    // aheader.AreaType = 0x00;		// 区域类型；动态区时，固定为0x10 =
    // aheader.AreaX = 0;              // 区域左上角横坐标
    // aheader.AreaY = 0;              // 区域左上角纵坐标
    // aheader.AreaWidth = 64;         // 区域宽度
    // aheader.AreaHeight = 256;		// 区域高度
    // aheader.BackGroundFlag = 0x00;  // 是否有背景
    // aheader.Transparency = 101;     // 透明度
    // aheader.AreaEqual = 0x00;       // 前景、背景区域大小是否相同
    // aheader.stSoundData = sound_6G;

    // // pheader
    // pheader.PageStyle = 0x00;       // 数据页类型
    // pheader.DisplayMode = 0x01;		// 显示方式:0x00 –随机显示 = ; 0x01–静止显示 = ; 0x02–快速打出 = ; 0x03–向左移动 = ; 0x04 –向左连移 = ;  0x25 –向右移动  0x26 –向右连移  0x27 –向下移动  0x28 –向下连移
    // pheader.ClearMode = 0x00;       // 退出方式/清屏方式
    // pheader.Speed = 0x02;			// 速度等级
    // pheader.StayTime = 200;			// 停留时间
    // pheader.RepeatTime = 1;         // 重复次数
    // pheader.CartoonFrameRate = 0x00;// 特技为动画方式时，该值代表其帧率
    // pheader.BackNotValidFlag = 0x00;// 背景无效标志
    // // 字体信息-------------------------------------------------------------------------------------------------------
    // pheader.arrMode = eMULTILINE;   // eSINGLELINE eMULTILINE
    // pheader.fontSize = fontSize;    // 字体大小
    // pheader.color = eRED;           // 字体颜色 E_Color_G56此通过此枚举值可以直接配置七彩色，如果大于枚举范围使用RGB888模式
    // pheader.fontBold = false;		// 是否为粗体
    // pheader.fontItalic = false;		// 是否为斜体
    // pheader.tdirection = pNORMAL;	// 文字方向
    // pheader.txtSpace = 1;			// 文字间隔
    // pheader.Halign = 2;             // 横向对齐方式（0系统自适应、1左对齐、2居中、3右对齐）
    // pheader.Valign = 2;             // 纵向对齐方式（0系统自适应、1上对齐、2居中、3下对齐）

    // if(0 != bxDual_program_addProgram_G6(&programH)) qDebug() << ("bxDual_program_addProgram_G6 error");
    // if(0 != bxDual_program_addArea_G6(areaID, &aheader)) qDebug() << ("bxDual_program_addArea_G6 error");
    // if(0 != bxDual_program_picturesAreaAddTxt_G6(areaID, (Ouint8*)content.toLocal8Bit().data(), (Ouint8*)font.toLocal8Bit().data(), &pheader)) qDebug() << ("bxDual_program_picturesAreaAddTxt_G6 error");
    // if(0 != bxDual_program_IntegrateProgramFile_G6(&program)) qDebug() << ("bxDual_program_IntegrateProgramFile_G6 error");
    // qDebug() << ("文件名：" + QLatin1String((char*)(program.fileName)));
    // if(0 != bxDual_cmd_uart_ofsStartFileTransf(m_portName.toLocal8Bit().data(), 2)) qDebug() << ("bxDual_cmd_ofsStartFileTransf error");
    // if(0 != bxDual_cmd_uart_ofsWriteFile(m_portName.toLocal8Bit().data(), 2, program.fileName,
    //                                  program.fileType, program.fileLen, 1, program.fileAddre)) qDebug() << ("bxDual_cmd_ofsWriteFile2 error");
    // if(0 != bxDual_cmd_uart_ofsWriteFile(m_portName.toLocal8Bit().data(), 2, program.dfileName,
    //                                  program.dfileType, program.dfileLen, 1, program.dfileAddre)) qDebug() << ("bxDual_cmd_ofsWriteFile2 error");
    // if(0 != bxDual_cmd_uart_ofsEndFileTransf(m_portName.toLocal8Bit().data(), 2)) qDebug() << ("bxDual_cmd_ofsEndFileTransf error");
}

void Screen::addProgram_G6()
{
    EQprogramHeader_G6 pHeader;
    pHeader.FileType = 0x00;
    pHeader.ProgramID = 0;
    pHeader.ProgramStyle=0x00;
    pHeader.ProgramPriority=0x00;
    pHeader.ProgramPlayTimes=1;
    pHeader.ProgramTimeSpan=0;
    pHeader.SpecialFlag = 0;
    pHeader.CommExtendParaLen = 0x00;
    pHeader.ScheduNum = 0;
    pHeader.LoopValue = 0;
    pHeader.Intergrate = 0x00;
    pHeader.TimeAttributeNum = 0x00;
    pHeader.TimeAttribute0Offset = 0x0000;
    pHeader.ProgramWeek=0xff;
    pHeader.ProgramLifeSpan_sy=0xffff;
    pHeader.ProgramLifeSpan_sm=0x03;
    pHeader.ProgramLifeSpan_sd=0x14;
    pHeader.ProgramLifeSpan_ey=0xffff;
    pHeader.ProgramLifeSpan_em=0x03;
    pHeader.ProgramLifeSpan_ed=0x14;
    //pHeader.PlayPeriodGrpNum=0;

    bxDual_program_addProgram_G6(&pHeader);
}

void Screen::addArea_G6(Ouint16 AreaID, Ouint8 AreaType, Ouint16 AreaX, Ouint16 AreaY, Ouint16 AreaWidth, Ouint16 AreaHeight)
{
    oAreaHeader_G6->AreaType = AreaType;
    oAreaHeader_G6->AreaX = AreaX;
    oAreaHeader_G6->AreaY = AreaY;
    oAreaHeader_G6->AreaWidth = AreaWidth;
    oAreaHeader_G6->AreaHeight = AreaHeight;
    bxDual_program_addArea_G6(AreaID, oAreaHeader_G6);
}

void Screen::addAreaPicture_G6(Ouint16 AreaID, QString content, QString font, int fontSize)
{
    stPageHeader->fontSize = fontSize;

    bxDual_program_picturesAreaAddTxt_G6(AreaID, (Ouint8*)content.toLocal8Bit().data(),
                                         (Ouint8*)font.toLocal8Bit().data(), stPageHeader);
}

void Screen::comSendProgram_G5(Oint8* uartPort, Ouint8 baudRate)
{
    qDebug() << uartPort << "  :  " << baudRate;
    EQprogram program;
    memset((void*)&program, 0, sizeof(program));
    bxDual_program_IntegrateProgramFile(&program);
    Oint8 ret;

    ret = bxDual_cmd_uart_ofsStartFileTransf(uartPort, baudRate);
    printf("tcp_send_program_G5L:cmd_ofsStartFileTransf===== %d \n", ret);
    if(ret != 0){
        printf("cmd_ofsStartFileTransf run error...");
    }else{
        printf("cmd_ofsStartFileTransf run succeed...");
    }

    ret = bxDual_cmd_uart_ofsWriteFile(uartPort, baudRate, program.fileName, program.fileType, program.fileLen, 1, program.fileAddre);
    if(ret != 0){
        printf("cmd_ofsWriteFile run error...");
    }else{
        printf("cmd_ofsWriteFile run succeed...");
    }
    printf("tcp_send_program_G5:cmd_ofsWriteFile===== %d \n", ret);
    printf("fileName_G5 == %s \n", program.fileName);
    printf("fileType_G5 == %d \n", program.fileType);
    printf("fileLen_G5 == %d \n", program.fileLen);
    printf("fileCRC32_G5 == %d \n",program.fileCRC32);
    ret = bxDual_cmd_uart_ofsEndFileTransf(uartPort, baudRate);
    if(ret != 0){
        printf("cmd_ofsEndFileTransf run error...");
    }else{
        printf("cmd_ofsEndFileTransf run succeed...");
    }
    printf("tcp_send_program_G5:md_ofsWriteFile===== %d \n", ret);

    // bxDual_program_deleteProgram();
    // bxDual_program_freeBuffer(&program);
}

void Screen::aaddProgram_G6()
{
    EQprogramHeader_G6 pHeader;
    pHeader.FileType = 0x00;
    pHeader.ProgramID = 0;
    pHeader.ProgramStyle=0x00;
    pHeader.ProgramPriority=0x00;
    pHeader.ProgramPlayTimes=1;
    pHeader.ProgramTimeSpan=0;
    pHeader.SpecialFlag = 0;
    pHeader.CommExtendParaLen = 0x00;
    pHeader.ScheduNum = 0;
    pHeader.LoopValue = 0;
    pHeader.Intergrate = 0x00;
    pHeader.TimeAttributeNum = 0x00;
    pHeader.TimeAttribute0Offset = 0x0000;
    pHeader.ProgramWeek=0xff;
    pHeader.ProgramLifeSpan_sy=0xffff;
    pHeader.ProgramLifeSpan_sm=0x03;
    pHeader.ProgramLifeSpan_sd=0x14;
    pHeader.ProgramLifeSpan_ey=0xffff;
    pHeader.ProgramLifeSpan_em=0x03;
    pHeader.ProgramLifeSpan_ed=0x14;
    //pHeader.PlayPeriodGrpNum=0;

    bxDual_program_addProgram_G6(&pHeader);
}

void Screen::aaddArea_G6(Ouint16 AreaID, Ouint8 AreaType, Ouint16 AreaX, Ouint16 AreaY, Ouint16 AreaWidth, Ouint16 AreaHeight)
{
    Ouint16 nAreaID = AreaID;
    EQareaHeader_G6 aHeader1;
    aHeader1.AreaType = AreaType;
    aHeader1.AreaX = AreaX;
    aHeader1.AreaY = AreaY;
    aHeader1.AreaWidth = AreaWidth;
    aHeader1.AreaHeight = AreaHeight;
    aHeader1.BackGroundFlag = 0x00;
    aHeader1.Transparency = 101;
    aHeader1.AreaEqual = 0x00;
    bxDual_program_addArea_G6(nAreaID, &aHeader1);
}

void Screen::aaddAreaPicture_G6(int color, int areaId, QString fontName, int fontSize,
                                QString content, int Halign, int Valign, int DisplayMode,
                                int Speed)
{
    EQpageHeader_G6 pheader1;
    pheader1.PageStyle = 0x00;
    pheader1.DisplayMode = DisplayMode;
    pheader1.ClearMode = 0x01;
    pheader1.Speed = Speed;
    pheader1.StayTime = 0;
    pheader1.RepeatTime = 1;
    pheader1.ValidLen = 1;
    pheader1.CartoonFrameRate = 0x00;
    pheader1.BackNotValidFlag = 0x00;
    pheader1.arrMode = eSINGLELINE; //eMULTILINE;//
    pheader1.fontSize = fontSize;
    pheader1.color = E_Color_G56(color);   // E_Color_G56
    pheader1.fontBold = false;
    pheader1.fontItalic = false;
    pheader1.tdirection = pNORMAL;
    pheader1.txtSpace = 0;
    pheader1.Valign = Valign;
    pheader1.Halign = Halign;

    bxDual_program_picturesAreaAddTxt_G6(areaId, (Ouint8*)content.toLocal8Bit().data(), (Ouint8*)fontName.toLocal8Bit().data(), &pheader1);
}

void Screen::com_send_program_G5(Oint8 *uartPort, Ouint8 baudRate)
{
    EQprogram program;
    memset((void*)&program, 0, sizeof(program));
    bxDual_program_IntegrateProgramFile(&program);
    Oint8 ret;

    ret = bxDual_cmd_uart_ofsStartFileTransf(uartPort, baudRate);
    printf("tcp_send_program_G5L:cmd_ofsStartFileTransf===== %d \n", ret);
    if(ret != 0){
        printf("cmd_ofsStartFileTransf run error...");
    }else{
        printf("cmd_ofsStartFileTransf run succeed...");
    }


    ret = bxDual_cmd_uart_ofsWriteFile(uartPort, baudRate, program.fileName, program.fileType, program.fileLen, 1, program.fileAddre);
    if(ret != 0){
        printf("cmd_ofsWriteFile run error...");
    }else{
        printf("cmd_ofsWriteFile run succeed...");
    }
    printf("tcp_send_program_G5:cmd_ofsWriteFile===== %d \n", ret);
    printf("fileName_G5 == %s \n", program.fileName);
    printf("fileType_G5 == %d \n", program.fileType);
    printf("fileLen_G5 == %d \n", program.fileLen);
    printf("fileCRC32_G5 == %d \n",program.fileCRC32);
    ret = bxDual_cmd_uart_ofsEndFileTransf(uartPort, baudRate);
    if(ret != 0){
        printf("cmd_ofsEndFileTransf run error...");
    }else{
        printf("cmd_ofsEndFileTransf run succeed...");
    }
    printf("tcp_send_program_G5:md_ofsWriteFile===== %d \n", ret);
    //ɾ�������ڴ��еĽ�Ŀ
    bxDual_program_deleteProgram();
    bxDual_program_freeBuffer(&program);
}

void Screen::slotTest(QByteArray jsonData)
{
    QJsonObject json = QJsonDocument::fromJson(jsonData).object();

    sendStaticProgram(json.value("content").toString(), json.value("font").toString(),
                      json.value("fontSize").toInt());
}

void Screen::slotDynamicArea_AddAreaTxtDetails_6G_serial(int nBaudRateIndex, int color,
                                                         int areaId, int areaX, int areaY, int width,
                                                         int height, QString fontName, int fontSize,
                                                         QString content, int Halign, int Valign, int DisplayMode, int Speed)
{
    // E_Color_G56::eBLACK
    // nBaudRateIndex: 9600 57600
    oAreaHeader_G6->AreaX = areaX;
    oAreaHeader_G6->AreaY = areaY;
    oAreaHeader_G6->AreaWidth = width;
    oAreaHeader_G6->AreaHeight = height;

    stPageHeader->color = (E_Color_G56)color;
    stPageHeader->fontSize = fontSize;
    stPageHeader->Halign = Halign;
    stPageHeader->Valign = Valign;
    stPageHeader->DisplayMode = DisplayMode;
    stPageHeader->Speed = Speed;

    int res = bxDual_dynamicArea_AddAreaTxtDetails_6G_Serial(m_portName.toLocal8Bit().data(),
                                                             nBaudRateIndex,
                                                             eSCREEN_COLOR_DOUBLE,
                                                             areaId,
                                                             oAreaHeader_G6,
                                                             stPageHeader,
                                                             (Ouint8*)fontName.toLocal8Bit().data(),
                                                             (Ouint8*)content.toLocal8Bit().data());

     // int res =  bxDual_dynamicArea_AddAreaTxtDetails_6G((Ouint8*)"192.168.1.176",
     //                                                   5005,
     //                                                   eSCREEN_COLOR_DOUBLE,
     //                                                   areaId,
     //                                                    oAreaHeader_G6,
     //                                                    stPageHeader,
     //                                                    (Ouint8*)fontName.toLocal8Bit().data(),
     //                                                    (Ouint8*)content.toLocal8Bit().data());
    qDebug() << m_portName;
    qDebug() << "串口发送动态区结果 : " << res;
}

void Screen::slotDynamicArea_DelArea_G5_Serial(int nBaudRateIndex, int areaId)
{
    qDebug() << m_portName;

    // Oint8 COMPort[] = "ttyS5";

    int res = -2;
    // res = bxDual_cmd_uart_searchController(m_udpPingRetData, COMPort);

    qDebug() << "bxDual_cmd_uart_searchController [res] : " << res;

    res = bxDual_dynamicArea_DelArea_G5_Serial(m_portName.toLocal8Bit().data(), nBaudRateIndex, areaId);
    // res = bxDual_dynamicArea_DelArea_5G((Ouint8*)"192.168.1.176",
    //                                     5005,
    //                                     255);
    qDebug() << "串口关闭动态区结果 : " << res;
}

void Screen::slotStaticProgram(int nBaudRateIndex, int color, int areaId, int areaX, int areaY,
                                int width, int height, QString fontName, int fontSize,
                                QString content, int Halign, int Valign, int DisplayMode,
                                int Speed)
{
    // aaddProgram_G6();
    // aaddArea_G6(areaId, 0, areaX, areaY, width, height);
    // aaddAreaPicture_G6(color, areaId, fontName, fontSize, content, Halign, Valign, DisplayMode, Speed);
    // com_send_program_G5(m_portName.toLocal8Bit().data(), nBaudRateIndex);

    BXSound_6G* sound_6G = new BXSound_6G;
    EQprogram_G6* program = new EQprogram_G6;

    //memset(&programH, 0, sizeof(EQprogramHeader_G6));
    // memset(&aheader, 0, sizeof(EQareaHeader_G6));
    // memset(&sound_6G, 0, sizeof(BXSound_6G));
    // memset(&pheader, 0, sizeof(EQpageHeader_G6));
    //memset(&program, 0, sizeof(EQprogram_G6));

    // programH
    EQprogramHeader_G6* programH = new EQprogramHeader_G6;
    programH->FileType = 0x00;
    programH->ProgramID = 0x00;
    programH->ProgramStyle = 0x00;
    programH->ProgramPriority = 0x00;
    programH->ProgramPlayTimes = 0x01;
    programH->ProgramTimeSpan = 0x00;
    programH->CommExtendParaLen = 0x00;
    programH->ScheduNum = 0x00;
    programH->LoopValue = 0x00;
    programH->Intergrate = 0x00;
    programH->TimeAttributeNum = 0x00;
    programH->TimeAttribute0Offset = 0x0000;
    programH->ProgramWeek = 0xff;
    programH->ProgramLifeSpan_sy = 0xffff;
    programH->ProgramLifeSpan_sm = 0x03;
    programH->ProgramLifeSpan_sd = 0x14;
    programH->ProgramLifeSpan_ey = 0xffff;
    programH->ProgramLifeSpan_em = 0x03;
    programH->ProgramLifeSpan_ed = 0x14;
    //programH->PlayPeriodGrpNum = 0;

    if(0 != bxDual_program_addProgram_G6(programH)) qDebug() << ("bxDual_program_addProgram_G6 error");

    // aheader
    EQareaHeader_G6* aheader = new EQareaHeader_G6;
    aheader->AreaType = 0x00;           // 区域类型；动态区时，固定为0x10 =
    aheader->AreaX = areaX;             // 区域左上角横坐标
    aheader->AreaY = areaY;             // 区域左上角纵坐标
    aheader->AreaWidth = width;         // 区域宽度
    aheader->AreaHeight = height;		// 区域高度
    aheader->BackGroundFlag = 0x00;     // 是否有背景
    aheader->Transparency = 101;        // 透明度
    aheader->AreaEqual = 0x00;          // 前景、背景区域大小是否相同
    aheader->stSoundData = *sound_6G;
    if(0 != bxDual_program_addArea_G6(areaId, aheader)) qDebug() << ("bxDual_program_addArea_G6 error");

    // pheader
    EQpageHeader_G6* pheader = new EQpageHeader_G6;
    pheader->PageStyle = 0x00;          // 数据页类型
    pheader->DisplayMode = DisplayMode; // 显示方式:0x00 –随机显示 = ; 0x01–静止显示 = ; 0x02–快速打出 = ; 0x03–向左移动 = ; 0x04 –向左连移 = ;  0x25 –向右移动  0x26 –向右连移  0x27 –向下移动  0x28 –向下连移
    pheader->ClearMode = 0x01;          // 退出方式/清屏方式
    pheader->Speed = Speed;             // 速度等级
    pheader->StayTime = 100;			// 停留时间
    pheader->RepeatTime = 0;            // 重复次数
    pheader->ValidLen = 10;
    pheader->CartoonFrameRate = 0x00;   // 特技为动画方式时，该值代表其帧率
    pheader->BackNotValidFlag = 0x00;   // 背景无效标志
    // 字体信息-------------------------------------------------------------------------------------------------------
    pheader->arrMode = eMULTILINE;      // eSINGLELINE eMULTILINE
    pheader->fontSize = fontSize;       // 字体大小
    pheader->color = E_Color_G56(color);// 字体颜色 E_Color_G56此通过此枚举值可以直接配置七彩色，如果大于枚举范围使用RGB888模式
    pheader->fontBold = false;          // 是否为粗体
    pheader->fontItalic = false;		// 是否为斜体
    pheader->tdirection = pNORMAL;      // 文字方向
    pheader->txtSpace = 1;              // 文字间隔
    pheader->Halign = Halign;           // 横向对齐方式（0系统自适应、1左对齐、2居中、3右对齐）
    pheader->Valign = Valign;           // 纵向对齐方式（0系统自适应、1上对齐、2居中、3下对齐）
    if(0 != bxDual_program_picturesAreaAddTxt_G6(areaId, (Ouint8*)content.toLocal8Bit().data(), (Ouint8*)fontName.toLocal8Bit().data(), pheader))  qDebug() << ("bxDual_program_picturesAreaAddTxt_G6 error");



    if(0 != bxDual_program_IntegrateProgramFile_G6(program)) qDebug() << ("bxDual_program_IntegrateProgramFile_G6 error");
    if(0 != bxDual_program_deleteProgram_G6())  qDebug() << ("bxDual_program_deleteProgram_G6 error");

    emit qDebug() << ("文件名：" + QLatin1String((char*)(program->fileName)));
    // qDebug() << " ************* " << ip;
    if(0 != bxDual_cmd_uart_ofsStartFileTransf(m_portName.toLocal8Bit().data(), nBaudRateIndex)) qDebug() << ("bxDual_cmd_ofsStartFileTransf error");
    if(0 != bxDual_cmd_uart_ofsWriteFile(m_portName.toLocal8Bit().data(), nBaudRateIndex, program->dfileName,
                                     program->dfileType, program->dfileLen, 1, program->dfileAddre)) qDebug() << ("bxDual_cmd_ofsWriteFile2 error");
    if(0 != bxDual_cmd_uart_ofsWriteFile(m_portName.toLocal8Bit().data(), nBaudRateIndex, program->fileName,
                                     program->fileType, program->fileLen, 1, program->fileAddre)) qDebug() << ("bxDual_cmd_ofsWriteFile2 error");
    if(0 != bxDual_cmd_uart_ofsEndFileTransf(m_portName.toLocal8Bit().data(), nBaudRateIndex)) qDebug() << ("bxDual_cmd_ofsEndFileTransf error");
}
