#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextCodec>
#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>

#include "myhttpserver.h"
#include "include/libhv/hthread.h"    // import hv_gettid
#include "include/libhv/hasync.h"     // import hv::async
#include "qaesencryption.h"

#define SRC_PATH "/src/dist"

MyHttpServer::MyHttpServer(QJsonObject& cfgJson, QObject *parent)
    : QObject(parent)
{
    createHttpserver(cfgJson.value("port").toInt());
}

MyHttpServer::~MyHttpServer()
{
    m_httpServer->stop();
    hv::async::cleanup();
}

void MyHttpServer::stop()
{
    m_httpServer->stop();
    hv::async::cleanup();
}

void MyHttpServer::createHttpserver(int port)
{
    HV_MEMCHECK;

    m_router = new HttpService;

    m_router->Static("/", "./html");
    m_router->EnableForwardProxy();
    m_router->AddTrustProxy("*httpbin.org");
    m_router->Proxy("/httpbin/", "http://httpbin.org/");

    m_router->POST("/echo", [](const HttpContextPtr& ctx) {
        return ctx->send(ctx->body(), ctx->type());
    });

    /*          GET            */
    m_router->GET("/ping", [](HttpRequest* req, HttpResponse* resp) {
        Q_UNUSED(req);

        QJsonObject backJson;
        backJson.insert("time", "最后更新时间：2025年07月19日");
        backJson.insert("Name", "尤特斯安全桩服务");
        backJson.insert("Version", "0.5.0");
        backJson.insert("Msg", "alpha");

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

    m_router->POST("/screen/setDefaultProgam", [this](HttpRequest* req, HttpResponse* resp) {

        //获取json数据包
        QJsonDocument jsonDoc = QJsonDocument::fromJson(QString::fromStdString(req->body).toUtf8());
        QJsonObject jsonObj = jsonDoc.object();


        // qDebug() << jsonDoc;

        QJsonObject backJson;
        backJson["code"] = 200;
        backJson["msg"] = "success";

        // 判断头
        if(!headerIsOk(req, backJson)){
            return respReturnJson(resp, backJson);
        }

        QJsonObject jsonProgram = parseScreenOpenControl(jsonObj, backJson);
        emit signalSetDefaultProgam(QJsonDocument(jsonProgram).toJson());

        return respReturnJson(resp, backJson);
    });

    // 管控接口
    m_router->POST("/screen/openControl", [this](HttpRequest* req, HttpResponse* resp) {

        //获取json数据包
        QJsonDocument jsonDoc = QJsonDocument::fromJson(QString::fromStdString(req->body).toUtf8());
        QJsonObject jsonObj = jsonDoc.object();


        // qDebug() << jsonDoc;

        QJsonObject backJson;
        backJson["code"] = 200;
        backJson["msg"] = "success";

        // 判断头
        if(!headerIsOk(req, backJson)){
            return respReturnJson(resp, backJson);
        }

        QJsonObject jsonProgram = parseScreenOpenControl(jsonObj, backJson);
        emit signalOpenControl(QJsonDocument(jsonProgram).toJson(), true);

        return respReturnJson(resp, backJson);
    });

    // 关闭管控接口
    m_router->POST("/screen/closeControl", [this](HttpRequest* req, HttpResponse* resp) {

        QJsonObject backJson;
        backJson["code"] = 200;
        backJson["msg"] = "success";

        // 判断头
        if(!headerIsOk(req, backJson)){
            return respReturnJson(resp, backJson);
        }

        emit signalOpenControl("", false);
        return respReturnJson(resp, backJson);
    });

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

bool MyHttpServer::ipAddrIsOK(const QString &ip)
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

QString MyHttpServer::qstr2Hex(QString instr)
{
    if(instr.isEmpty()) return instr;

    QTextCodec *codec = QTextCodec::codecForName("GB2312");  //set char code
    QByteArray encodedString = codec->fromUnicode(instr);//convert to unicode
    instr = encodedString.toHex(QChar::Space); //hex add space example:FFAC21 convert to FF AC 21
    return instr.toUpper();
}

void MyHttpServer::add_file_handler(HttpServer &server, const QString &basepath, const QString &path)
{
    QString filepath = basepath + "/" + path;
    QFileInfo fileInfo(filepath);
    if (fileInfo.isFile()) {
        m_router->GET(("/"+path).toStdString().c_str(), [filepath](HttpRequest* req, HttpResponse* resp) {

            return resp->File(filepath.toStdString().c_str());
        });
    }
}

void MyHttpServer::add_directory_handlers(HttpServer &server, const QString &basepath, const QString &path)
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

QByteArray MyHttpServer::crypt_Aes128_ECB_PKCS7_HEX(QByteArray plaintext, QByteArray key)
{
    return QAESEncryption::Crypt(QAESEncryption::AES_128, QAESEncryption::ECB, plaintext, key, "", QAESEncryption::PKCS7).toHex();
}

QByteArray MyHttpServer::decrypt_Aes128_ECB_PKCS7_HEX(QByteArray plaintext, QByteArray key)
{
    return QAESEncryption::Decrypt(QAESEncryption::AES_128, QAESEncryption::ECB, plaintext, key, "", QAESEncryption::PKCS7);
}

bool MyHttpServer::headerIsOk(HttpRequest *req, QJsonObject &backJson)
{
    if(QString::fromStdString(req->GetHeader("Content-Type")).toLower() != "application/json"){
        backJson["code"] = 400;
        backJson["msg"] = "请求头需要 application/json";
        return false;
    }
    return true;
}

int MyHttpServer::respReturnJson(HttpResponse *resp, QJsonObject json)
{
    resp->content_type = APPLICATION_JSON;
    resp->body = QJsonDocument(json).toJson().toStdString().c_str();
    return 200;
}

bool MyHttpServer::missingScreenOpenControl(const QJsonObject &json, QJsonObject &backJson)
{
    // color
    if(json.find("color") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 color ";
        return true;
    }else if(!json.value("color").isDouble()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "color 数据类型错误 应该为 int";
        return true;
    }
    int color = json.value("color").toInt();
    if(color < 1 || color > 3){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "color 数据值只能为  1-红 2-绿 3-黄";
        return true;
    }

    // signalScreen
    if(json.find("signalScreen") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "当文字数量8时 缺少必要参数 signalScreen 用来控制单双屏显示";
        return true;
    }else if(!json.value("signalScreen").isBool()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "signalScreen 数据类型错误 应该为 bool";
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
    QString content = json.value("content").toString();
    int contentSize = content.size();

    // signalScreen 与 content 联系
    if(json.value("signalScreen").toBool()){
        if(contentSize != 2 && contentSize != 12 && contentSize != 16){
            if(contentSize < 4 || contentSize > 8){
                backJson.find("code").value() = 1;
                backJson.find("msg").value() = "单屏时 content 数据数量只能为 2/4/5/6/7/8/12/16";
                return true;
            }
        }
    }else{
        if(contentSize != 4 && contentSize != 8 && contentSize != 10 && contentSize != 12 && contentSize != 14){
            backJson.find("code").value() = 1;
            backJson.find("msg").value() = "双屏时 content 数据数量只能为 4/8/10/12/14";
            return true;
        }
    }

    return false;
}

QJsonObject MyHttpServer::parseScreenOpenControl(const QJsonObject &json, QJsonObject &backJson)
{
    // 判断是否缺失参数
    if(missingScreenOpenControl(json, backJson)){
        return backJson;
    }

    QString content = json.value("content").toString();
    int fontSize = 1;
    int contentSize = content.size();
    bool signalScreen = json.value("signalScreen").toBool();
    QString left, right;

    if(signalScreen){
        switch(contentSize){
        case 2:
            fontSize = 48;
            content.insert(1, "  ");
            break;
        case 4:
            fontSize = 45;
            break;
        case 5:
            fontSize = 36;
            break;
        case 6:
            fontSize = 29;
            break;
        case 7:
            fontSize = 25;
            break;
        case 8:
            for(int i = 0; i<contentSize; i++){
                content.insert(i + i*1, "\n");
            }
            content = content.trimmed();
            fontSize = 22;
            break;
        case 12:
            left = content.left(content.size()/2);
            right = content.right(content.size()/2);
            content.clear();
            for (int i=0; i<left.size(); i++) {
                content += left.at(i);
                content += right.at(i);
            }
            fontSize = 24;
            break;
        case 16:
            left = content.left(content.size()/2);
            right = content.right(content.size()/2);
            content.clear();
            for (int i=0; i<left.size(); i++) {
                content += left.at(i);
                content += right.at(i);
            }
            fontSize = 22;
            break;
        default:
            break;
        }
    }else{
        switch(contentSize){
        case 4:
            fontSize = 48;
            content.insert(3, "  ");
            content.insert(1, "  ");
            break;
        case 8:
            fontSize = 45;
            break;
        case 10:
            fontSize = 36;
            break;
        case 12:
            fontSize = 29;
            break;
        case 14:
            fontSize = 25;
            break;
        default:
            break;
        }
    }

    QJsonObject jsonData;
    jsonData.insert("nBaudRateIndex", 2);
    jsonData.insert("color", json.value("color").toInt());
    jsonData.insert("areaId", 0);
    jsonData.insert("areaX", 0);
    jsonData.insert("areaY", 0);
    jsonData.insert("areaWidth", 64);
    jsonData.insert("areaHeight", 256);
    jsonData.insert("fontName", "等线");
    jsonData.insert("fontSize", fontSize);
    jsonData.insert("content", content);
    jsonData.insert("Halign", 2);
    jsonData.insert("Valign", 2);
    jsonData.insert("DisplayMode", 2);
    jsonData.insert("Speed", 1);

    return jsonData;
}
