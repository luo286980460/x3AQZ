#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextCodec>
#include <QFile>
#include <QCoreApplication>
#include <QDir>

#include "myhttpserveronbon.h"
#include "include/libhv/hthread.h"    // import hv_gettid
#include "include/libhv/hasync.h"     // import hv::async
#include "qaesencryption.h"

#define LOGIN_PATH "/src/dist/index.html"
#define SRC_PATH "/src/dist"
#define ACCOUNT_INFO_PATH "/aip.ls"


MyHttpServerOnBon::MyHttpServerOnBon(int port, QJsonObject& cfgJson, QObject *parent)
    : QObject(parent)
{
    createHttpserver(port);
}

MyHttpServerOnBon::~MyHttpServerOnBon()
{
    m_httpServer->stop();
    hv::async::cleanup();
}

void MyHttpServerOnBon::stop()
{
    m_httpServer->stop();
    hv::async::cleanup();
}

void MyHttpServerOnBon::createHttpserver(int port)
{
    HV_MEMCHECK;

    m_router = new HttpService;

    /* Static file service */
    // curl -v http://ip:port/
    m_router->Static("/", "./html");

    /* Forward proxy service */
    m_router->EnableForwardProxy();
    // curl -v http://httpbin.org/get --proxy http://127.0.0.1:8080
    m_router->AddTrustProxy("*httpbin.org");

    /* Reverse proxy service */
    // curl -v http://ip:port/httpbin/get
    m_router->Proxy("/httpbin/", "http://httpbin.org/");

    /*          POST            */
    // curl -v http://ip:port/echo -d "hello,world!"
    m_router->POST("/echo", [](const HttpContextPtr& ctx) {
        return ctx->send(ctx->body(), ctx->type());
    });


    /*          POST            */
    m_router->POST("/screen/text2Dynamic", [this](HttpRequest* req, HttpResponse* resp) {

        //获取json数据包
        QJsonDocument jsonDoc = QJsonDocument::fromJson(QString::fromStdString(req->body).toUtf8());
        QJsonObject jsonObj = jsonDoc.object();


        qDebug() << jsonDoc;

        QJsonObject backJson;
        backJson["code"] = 200;
        backJson["msg"] = "success";

        // 判断头
        if(!headerIsOk(req, backJson)){
            return respReturnJson(resp, backJson);
        }

        return respReturnJson(resp, parseScreenText2Dynamic(jsonObj, backJson));
    });

    m_router->POST("/screen/text2Static", [this](HttpRequest* req, HttpResponse* resp) {

        //获取json数据包
        QJsonDocument jsonDoc = QJsonDocument::fromJson(QString::fromStdString(req->body).toUtf8());
        QJsonObject jsonObj = jsonDoc.object();


        qDebug() << jsonDoc;

        QJsonObject backJson;
        backJson["code"] = 200;
        backJson["msg"] = "success";

        // 判断头
        if(!headerIsOk(req, backJson)){
            return respReturnJson(resp, backJson);
        }

        return respReturnJson(resp, parseScreenText2Static(jsonObj, backJson));
    });

    m_router->POST("/screen/setBrightness", [this](HttpRequest* req, HttpResponse* resp) {

        //获取json数据包
        QJsonDocument jsonDoc = QJsonDocument::fromJson(QString::fromStdString(req->body).toUtf8());
        QJsonObject jsonObj = jsonDoc.object();

        QJsonObject backJson;
        backJson["code"] = 200;
        backJson["msg"] = "success";

        // 判断头
        if(!headerIsOk(req, backJson)){
            return respReturnJson(resp, backJson);
        }

        return respReturnJson(resp, parseScreenSetBrightness(jsonObj, backJson));
    });


    m_router->POST("/screen/test", [this](HttpRequest* req, HttpResponse* resp) {

        //获取json数据包
        QJsonDocument jsonDoc = QJsonDocument::fromJson(QString::fromStdString(req->body).toUtf8());
        QJsonObject jsonObj = jsonDoc.object();


        qDebug() << jsonDoc;

        QJsonObject backJson;
        backJson["code"] = 200;
        backJson["msg"] = "success";

        emit signalTest(jsonDoc.toJson());

        return respReturnJson(resp, backJson);
    });

    m_router->POST("/screen/picToDynamic", [this](HttpRequest* req, HttpResponse* resp) {

        //获取json数据包
        QJsonDocument jsonDoc = QJsonDocument::fromJson(QString::fromStdString(req->body).toUtf8());
        QJsonObject jsonObj = jsonDoc.object();


        // qDebug() << jsonDoc;

        QJsonObject backJson;
        backJson["code"] = 200;
        backJson["msg"] = "待实现";
        return respReturnJson(resp, backJson);

        // // 判断头
        // if(!headerIsOk(req, backJson)){
        //     return respReturnJson(resp, backJson);
        // }

        // return respReturnJson(resp, parseLightProducer(jsonObj, backJson));
    });

    m_router->POST("/screen/closeDynamic", [this](HttpRequest* req, HttpResponse* resp) {
        //获取json数据包
        QJsonDocument jsonDoc = QJsonDocument::fromJson(QString::fromStdString(req->body).toUtf8());
        QJsonObject jsonObj = jsonDoc.object();


        qDebug() << jsonDoc;

        QJsonObject backJson;
        backJson["code"] = 200;
        backJson["msg"] = "success";

        // 判断头
        if(!headerIsOk(req, backJson)){
            return respReturnJson(resp, backJson);
        }

        return respReturnJson(resp, parseScreenCloseDynamic(jsonObj, backJson));
    });
    /*          GET            */
    /* API handlers */
    // curl -v http://ip:port/ping
    m_router->GET("/ping", [](HttpRequest* req, HttpResponse* resp) {
        Q_UNUSED(req);

        QJsonObject backJson;
        backJson.insert("time", "最后更新时间：2025年07月04日");
        backJson.insert("Name", "尤特斯仰邦控制卡控屏服务");
        backJson.insert("Version", "0.1.0");
        backJson.insert("Msg", "串口控屏实现");

        resp->content_type = APPLICATION_JSON;
        resp->body = QJsonDocument(backJson).toJson().toStdString();


        // resp->body = aes128_ECB_PKCS7_HEX(QJsonDocument(backJson).toJson(), m_aesKey).toStdString();
        return 200;
        // return resp->Json(ex3);
    });

    // curl -v http://ip:port/paths
    m_router->GET("/paths", [this](HttpRequest* req, HttpResponse* resp) {
        Q_UNUSED(req);
        return resp->Json(m_router->Paths());
    });

    // curl -v http://ip:port/get?env=1
    m_router->GET("/get", [](const HttpContextPtr& ctx) {
        hv::Json resp;
        resp["origin"] = ctx->ip();
        resp["url"] = ctx->url();
        resp["args"] = ctx->params();
        resp["headers"] = ctx->headers();
        return ctx->send(resp.dump(2));
    });

    // curl -v http://ip:port/user/123
    m_router->GET("/user/{id}", [](const HttpContextPtr& ctx) {
        hv::Json resp;
        resp["id"] = ctx->param("id");
        return ctx->send(resp.dump(2));
    });

    // curl -v http://ip:port/async
    m_router->GET("/async", [](const HttpRequestPtr& req, const HttpResponseWriterPtr& writer) {
        Q_UNUSED(req);
        writer->Begin();
        writer->WriteHeader("X-Response-tid", hv_gettid());
        writer->WriteHeader("Content-Type", "text/plain");
        writer->WriteBody("This is an async response.\n");
        writer->End();
    });

    // web 模块


    m_httpServer = new HttpServer;


    add_directory_handlers(*m_httpServer, QCoreApplication::applicationDirPath() + SRC_PATH);

    m_httpServer->service = m_router;
    m_httpServer->port = port;

#if TEST_HTTPS
    server.https_port = 8443;
    hssl_ctx_opt_t param;
    memset(&param, 0, sizeof(param));
    param.crt_file = "cert/server.crt";
    param.key_file = "cert/server.key";
    param.endpoint = HSSL_SERVER;
    if (server.newSslCtx(&param) != 0) {
        fprintf(stderr, "new SSL_CTX failed!\n");
        return -20;
    }
#endif

    // uncomment to test multi-processes
    // server.setProcessNum(4);
    // uncomment to test multi-threads
    // server.setThreadNum(4);


    m_httpServer->start();
}

bool MyHttpServerOnBon::ipAddrIsOK(const QString &ip)
{
    if (ip.isEmpty())
    {
        return false;
    }

    QStringList list = ip.split('.');
    if (list.size() != 4)
    {
        return false;
    }

    for (const auto& num : list)
    {
        bool ok = false;
        int temp = num.toInt(&ok);
        if (!ok || temp < 0 || temp > 255)
        {
            return false;
        }
    }

    return true;
}

QString MyHttpServerOnBon::qstr2Hex(QString instr)
{
    if(instr.isEmpty()) return instr;

    QTextCodec *codec = QTextCodec::codecForName("GB2312");  //set char code
    QByteArray encodedString = codec->fromUnicode(instr);//convert to unicode
    instr = encodedString.toHex(QChar::Space); //hex add space example:FFAC21 convert to FF AC 21
    return instr.toUpper();
}

void MyHttpServerOnBon::add_file_handler(HttpServer &server, const QString &basepath, const QString &path)
{
    Q_UNUSED(server);
    QString filepath = basepath + "/" + path;
    QFileInfo fileInfo(filepath);
    if (fileInfo.isFile()) {
        m_router->GET(("/"+path).toStdString().c_str(), [filepath](HttpRequest* req, HttpResponse* resp) {
            Q_UNUSED(req);
            return resp->File(filepath.toStdString().c_str());
        });
    }
}

void MyHttpServerOnBon::add_directory_handlers(HttpServer &server, const QString &basepath, const QString &path)
{
    QDir dir(basepath + "/" + path);
    if (dir.exists()) {
        for (const QString& entry : dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
            QString subpath = path.isEmpty() ? entry : path + "/" + entry;
            if (QFileInfo(dir, entry).isDir()) {
                add_directory_handlers(server, basepath, subpath);
            } else {
                add_file_handler(server, basepath, subpath);
            }
        }
    }
}

QByteArray MyHttpServerOnBon::crypt_Aes128_ECB_PKCS7_HEX(QByteArray plaintext, QByteArray key)
{
    return QAESEncryption::Crypt(QAESEncryption::AES_128, QAESEncryption::ECB, plaintext, key, "", QAESEncryption::PKCS7).toHex();
}

QByteArray MyHttpServerOnBon::decrypt_Aes128_ECB_PKCS7_HEX(QByteArray plaintext, QByteArray key)
{
    return QAESEncryption::Decrypt(QAESEncryption::AES_128, QAESEncryption::ECB, plaintext, key, "", QAESEncryption::PKCS7);
}

bool MyHttpServerOnBon::headerIsOk(HttpRequest *req, QJsonObject &backJson)
{
    if(QString::fromStdString(req->GetHeader("Content-Type")).toLower() != "application/json"){
        backJson["code"] = 400;
        backJson["msg"] = "请求头需要 application/json";
        return false;
    }
    return true;
}

int MyHttpServerOnBon::respReturnJson(HttpResponse *resp, QJsonObject json)
{
    resp->content_type = APPLICATION_JSON;
    resp->body = QJsonDocument(json).toJson().toStdString().c_str();
    return 200;
}

bool MyHttpServerOnBon::missingParameterText2Dynamic(const QJsonObject &json, QJsonObject &backJson)
{
    // nBaudRateIndex
    if(json.find("nBaudRateIndex") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 nBaudRateIndex ";
        return true;
    }else if(!json.value("nBaudRateIndex").isDouble()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "nBaudRateIndex 数据类型错误 应该为 int";
        return true;
    }else if(json.value("nBaudRateIndex").toInt() != 1 && json.value("nBaudRateIndex").toInt() != 2){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "nBaudRateIndex 只能为 1/2 [1=9600/2=57600]";
        return true;
    }

    // color
    if(json.find("color") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 color ";
        return true;
    }else if(!json.value("color").isDouble()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "color 数据类型错误 应该为 int";
        return true;
    }else if(json.value("color").toInt() != 1
               && json.value("color").toInt() != 2
               && json.value("color").toInt() != 3){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "color 只能为 1/2/3 [1=red/2=green/3=yellow]";
        return true;
    }

    // areaId
    if(json.find("areaId") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 areaId ";
        return true;
    }else if(!json.value("areaId").isDouble()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "areaId 数据类型错误 应该为 int";
        return true;
    }else if(json.value("areaId").toInt() < 0){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "areaId 必须 >= 0]";
        return true;
    }

    // areaX
    if(json.find("areaX") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 areaX ";
        return true;
    }else if(!json.value("areaX").isDouble()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "areaX 数据类型错误 应该为 int";
        return true;
    }

    // areaY
    if(json.find("areaY") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 areaY ";
        return true;
    }else if(!json.value("areaY").isDouble()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "areaY 数据类型错误 应该为 int";
        return true;
    }

    // areaWidth
    if(json.find("areaWidth") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 areaWidth ";
        return true;
    }else if(!json.value("areaWidth").isDouble()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "areaWidth 数据类型错误 应该为 int";
        return true;
    }

    // areaHeight
    if(json.find("areaHeight") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 areaHeight ";
        return true;
    }else if(!json.value("areaHeight").isDouble()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "areaHeight 数据类型错误 应该为 int";
        return true;
    }

    // fontName
    if(json.find("fontName") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 fontName ";
        return true;
    }else if(!json.value("fontName").isString()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "fontName 数据类型错误 应该为 string";
        return true;
    }

    // fontSize
    if(json.find("fontSize") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 fontSize ";
        return true;
    }else if(!json.value("fontSize").isDouble()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "fontSize 数据类型错误 应该为 int";
        return true;
    }

    // content
    if(json.find("content") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 content ";
        return true;
    }else if(!json.value("content").isString()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "content 数据类型错误 应该为 string";
        return true;
    }

    // Halign
    if(json.find("Halign") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 Halign ";
        return true;
    }else if(!json.value("Halign").isDouble()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "Halign 数据类型错误 应该为 int";
        return true;
    }
    int Halign = json.value("Halign").toInt();
    if(Halign != 1 && Halign != 2 && Halign != 3){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "Halign 横向对齐方式（0系统自适应、1左对齐、2居中、3右对齐）";
        return true;
    }

    // Valign
    if(json.find("Valign") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 Valign ";
        return true;
    }else if(!json.value("Valign").isDouble()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "Valign 数据类型错误 应该为 int";
        return true;
    }
    int Valign = json.value("Valign").toInt();
    if(Valign != 1 && Valign != 2 && Valign != 3){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "Valign 纵向对齐方式（0系统自适应、1上对齐、2居中、3下对齐）";
        return true;
    }

    // DisplayMode
    if(json.find("DisplayMode") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 DisplayMode ";
        return true;
    }else if(!json.value("DisplayMode").isDouble()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "DisplayMode 数据类型错误 应该为 int";
        return true;
    }else if(json.value("DisplayMode").toInt() < 0 || json.value("DisplayMode").toInt() > 2 ){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "DisplayMode 必须 0 ~ 2]";
        return true;
    }

    // Speed
    if(json.find("Speed") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 Speed ";
        return true;
    }else if(!json.value("Speed").isDouble()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "Speed 数据类型错误 应该为 int";
        return true;
    }else if(json.value("Speed").toInt() < 1){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "Speed 必须 >= 1]";
        return true;
    }
    return false;
}

QJsonObject MyHttpServerOnBon::parseScreenText2Dynamic(QJsonObject json, QJsonObject &backJson)
{
    if(missingParameterText2Dynamic(json, backJson)){
        return backJson;
    }

    emit signalDynamicArea_AddAreaTxtDetails_6G_serial(json.value("nBaudRateIndex").toInt(),
                                                       json.value("color").toInt(),
                                                       json.value("areaId").toInt(),
                                                       json.value("areaX").toInt(),
                                                       json.value("areaY").toInt(),
                                                       json.value("areaWidth").toInt(),
                                                       json.value("areaHeight").toInt(),
                                                       json.value("fontName").toString(),
                                                       json.value("fontSize").toInt(),
                                                       json.value("content").toString(),
                                                       json.value("Halign").toInt(),
                                                       json.value("Valign").toInt(),
                                                       json.value("DisplayMode").toInt(),
                                                       json.value("Speed").toInt());

    return backJson;
}

bool MyHttpServerOnBon::missingParameterCloseDynamic(const QJsonObject &json, QJsonObject &backJson)
{
    // nBaudRateIndex
    if(json.find("nBaudRateIndex") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 nBaudRateIndex ";
        return true;
    }else if(!json.value("nBaudRateIndex").isDouble()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "nBaudRateIndex 数据类型错误 应该为 int";
        return true;
    }else if(json.value("nBaudRateIndex").toInt() != 1 && json.value("nBaudRateIndex").toInt() != 2){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "nBaudRateIndex 只能为 1/2 [1=9600/2=57600]";
        return true;
    }

    // areaId
    if(json.find("areaId") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 areaId ";
        return true;
    }else if(!json.value("areaId").isDouble()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "areaId 数据类型错误 应该为 int";
        return true;
    }else if(json.value("areaId").toInt() < 0){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "areaId 必须 >= 0]";
        return true;
    }

    return false;
}

QJsonObject MyHttpServerOnBon::parseScreenCloseDynamic(QJsonObject json, QJsonObject &backJson)
{
    if(missingParameterCloseDynamic(json, backJson)){
        return backJson;
    }

    emit signalDynamicArea_DelArea_G5_Serial(json.value("nBaudRateIndex").toInt(),
                                                       json.value("areaId").toInt());

    return backJson;
}

QJsonObject MyHttpServerOnBon::parseScreenText2Static(QJsonObject json, QJsonObject &backJson)
{
    if(missingParameterCloseDynamic(json, backJson)){
        return backJson;
    }
    emit signalStaticProgram(json.value("nBaudRateIndex").toInt(),
                             json.value("color").toInt(),
                             json.value("areaId").toInt(),
                             json.value("areaX").toInt(),
                             json.value("areaY").toInt(),
                             json.value("areaWidth").toInt(),
                             json.value("areaHeight").toInt(),
                             json.value("fontName").toString(),
                             json.value("fontSize").toInt(),
                             json.value("content").toString(),
                             json.value("Halign").toInt(),
                             json.value("Valign").toInt(),
                             json.value("DisplayMode").toInt(),
                             json.value("Speed").toInt());
    return backJson;
}

QJsonObject MyHttpServerOnBon::parseScreenSetBrightness(QJsonObject json, QJsonObject &backJson)
{
    // nBaudRateIndex
    if(json.find("nBaudRateIndex") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 nBaudRateIndex ";
        return backJson;
    }else if(!json.value("nBaudRateIndex").isDouble()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "nBaudRateIndex 数据类型错误 应该为 int";
        return backJson;
    }else if(json.value("nBaudRateIndex").toInt() != 1 && json.value("nBaudRateIndex").toInt() != 2){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "nBaudRateIndex 只能为 1/2 [1=9600/2=57600]";
        return backJson;
    }

    // brightness
    int brightness = json.value("brightness").toInt();
    if(json.find("brightness") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 brightness ";
        return backJson;
    }else if(!json.value("brightness").isDouble()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "brightness 数据类型错误 应该为 int";
        return backJson;
    }else if(brightness < 0 || brightness > 15){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "brightness 只能为 0~255]";
        return backJson;
    }

    emit signalSetLuminance(brightness);
    backJson.find("code").value() = 0;
    backJson.find("msg").value() = "brightness 设置成功";
    return backJson;

}
