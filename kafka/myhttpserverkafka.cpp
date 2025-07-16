#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextCodec>
#include <QFile>
#include <QCoreApplication>
#include <QDir>

#include "myhttpserverkafka.h"
#include "include/libhv/hthread.h"    // import hv_gettid
#include "include/libhv/hasync.h"     // import hv::async
#include "producer.h"
#include "qaesencryption.h"

#define LOGIN_PATH "/src/dist/index.html"
#define SRC_PATH "/src/dist"
#define ACCOUNT_INFO_PATH "/aip.ls"


MyHttpServerKafka::MyHttpServerKafka(int port, QJsonObject& cfgJson, QObject *parent)
    : QObject(parent)
{
    createHttpserver(port);

    initProducer(cfgJson);
}

MyHttpServerKafka::~MyHttpServerKafka()
{
    m_httpServer->stop();
    hv::async::cleanup();
}

void MyHttpServerKafka::stop()
{
    m_httpServer->stop();
    hv::async::cleanup();
}

void MyHttpServerKafka::createHttpserver(int port)
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
    m_router->POST("/kafka/producer", [this](HttpRequest* req, HttpResponse* resp) {

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

        return respReturnJson(resp, parseLightProducer(jsonObj, backJson));
    });

    /*          GET            */
    m_router->GET("/ping", [](HttpRequest* req, HttpResponse* resp) {
        Q_UNUSED(req);

        QJsonObject backJson;
        backJson.insert("time", "最后更新时间：2025年01月04日");
        backJson.insert("Name", "尤特斯kafka创造者服务");
        backJson.insert("Version", "0.1.0");
        backJson.insert("Msg", "基础功能已实现");

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

bool MyHttpServerKafka::ipAddrIsOK(const QString &ip)
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

QString MyHttpServerKafka::qstr2Hex(QString instr)
{
    if(instr.isEmpty()) return instr;

    QTextCodec *codec = QTextCodec::codecForName("GB2312");  //set char code
    QByteArray encodedString = codec->fromUnicode(instr);//convert to unicode
    instr = encodedString.toHex(QChar::Space); //hex add space example:FFAC21 convert to FF AC 21
    return instr.toUpper();
}

void MyHttpServerKafka::add_file_handler(HttpServer &server, const QString &basepath, const QString &path)
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

void MyHttpServerKafka::add_directory_handlers(HttpServer &server, const QString &basepath, const QString &path)
{
    Q_UNUSED(server);
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

QByteArray MyHttpServerKafka::crypt_Aes128_ECB_PKCS7_HEX(QByteArray plaintext, QByteArray key)
{
    return QAESEncryption::Crypt(QAESEncryption::AES_128, QAESEncryption::ECB, plaintext, key, "", QAESEncryption::PKCS7).toHex();
}

QByteArray MyHttpServerKafka::decrypt_Aes128_ECB_PKCS7_HEX(QByteArray plaintext, QByteArray key)
{
    return QAESEncryption::Decrypt(QAESEncryption::AES_128, QAESEncryption::ECB, plaintext, key, "", QAESEncryption::PKCS7);
}

bool MyHttpServerKafka::headerIsOk(HttpRequest *req, QJsonObject &backJson)
{
    if(QString::fromStdString(req->GetHeader("Content-Type")).toLower() != "application/json"){
        qDebug() << QString::fromStdString(req->GetHeader("Content-Type")) ;
        backJson["code"] = 400;
        backJson["msg"] = "请求头需要 application/json";
        return false;
    }
    return true;
}

int MyHttpServerKafka::respReturnJson(HttpResponse *resp, QJsonObject json)
{
    resp->content_type = APPLICATION_JSON;
    resp->body = QJsonDocument(json).toJson().toStdString().c_str();
    return 200;
}

void MyHttpServerKafka::initProducer(QJsonObject &cfgJson)
{
    QJsonObject kafkaCfg = cfgJson.value("kafka").toObject();
    QString ipPort = kafkaCfg.value("ipPort").toString();
    QJsonArray topicsArray = kafkaCfg.value("topics").toArray();

    for(int i=0; i<topicsArray.size(); i++){
        Producer* producer = new Producer(ipPort, topicsArray.at(i).toString(), this);
        producer->start();
        m_producerList << producer;
    }
}

Producer *MyHttpServerKafka::getProducerFromTopic(const QString& topic)
{
    for(int i=0; i<m_producerList.size(); i++){
        if(m_producerList.at(i)->getTopic() == topic){
            return m_producerList.at(i);
        }
    }
    return nullptr;
}

bool MyHttpServerKafka::missingParameterProducer(const QJsonObject& json, QJsonObject& backJson)
{

    backJson.find("code").value() = 1;
    // topic
    if(json.find("topic") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 topic ";
        return true;
    }else if(!json.value("topic").isString()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "topic 数据类型错误 应该为 string";
        return true;
    }

    // key
    if(json.find("key") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 key ";
        return true;
    }else if(!json.value("key").isString()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "key 数据类型错误 应该为 string";
        return true;
    }

    // data
    if(json.find("data") == json.end()) {
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "缺少必要参数 data ";
        return true;
    }else if(!json.value("data").isObject()){
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "data 数据类型错误 应该为 json";
        return true;
    }

    return false;
}

QJsonObject MyHttpServerKafka::parseLightProducer(const QJsonObject &json, QJsonObject& backJson)
{
    // 判断是否缺失参数
    if(missingParameterProducer(json, backJson)){
        return backJson;
    }

    QString topic = json.value("topic").toString();
    Producer* producer = getProducerFromTopic(topic);

    if(producer){
        emit producer->signalProducerMsgJson(QJsonDocument(json.value("data").toObject()).toJson(),
                                             json.value("key").toString());
    }else{
        backJson.find("code").value() = 1;
        backJson.find("msg").value() = "producer 主题未找到 对象为空";
        return backJson;
    }

    return backJson;
}
