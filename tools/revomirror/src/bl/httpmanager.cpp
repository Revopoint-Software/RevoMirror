#include "httpmanager.h"
#include <QNetworkRequest>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUrl>
#include <QDebug>
#include <QNetworkReply>

#include "src/comm/httprequest.h"
#include "src/util/revoutils.h"

HttpManager::HttpManager(QObject *parent)
    : QObject(parent)
    , m_httpRequest(new HttpRequest())
{
    m_httpRequest->setServerAddress(SUNSHINE_WEB_SERVER_ADDRESS);
    m_httpRequest->setAuthorization(getAuthorization());
    m_httpRequest->setSSLEnable(true);

    connect(this, &HttpManager::signalInitUserComplete, this, &HttpManager::slotInitUserComplete);
    connect(this, &HttpManager::signalInitComplete, this, &HttpManager::startCheckConnectState);
    connect(&m_timer, &QTimer::timeout, this, &HttpManager::checkConnectState);

    m_timer.setInterval(500);
}

HttpManager::~HttpManager()
{

}

void HttpManager::init()
{
    QTimer::singleShot(SUNSHINE_INIT_TIMEOUT, [this]{
        m_initTimeout = true;
    });
    initHttpUser();
}

void HttpManager::stopMirror()
{
    QString url = "/api/apps/close";

    QJsonObject body;

    HttpRequest::RequestConfig config;
    config.onSuccess = [this](QNetworkReply*, const QByteArray& response){
        emit signalStopMirrorComplete(true);
    };
    config.onError = [this](QNetworkReply* reply, QNetworkReply::NetworkError){
        qDebug() << "HttpManager::stopMirror false, error info " << reply->errorString();
        emit signalQuitMirror(false);
    };


    m_httpRequest->post(url, body, config);
}

void HttpManager::quitMirrorService()
{
    m_timer.stop();

    QString url = "/api/quit";

    QJsonObject body;

    HttpRequest::RequestConfig config;
    config.onSuccess = [this](QNetworkReply*, const QByteArray& response){
        qDebug() << "HttpManager::slotQuitMirror success, response " << response;
        QJsonObject json = QJsonDocument::fromJson(response).object();
        int success = false;
        if(json.contains("status") && true == json.value("status").toBool())
        {
            success = true;
        }
        emit signalQuitMirror(success);
    };
    config.onError = [this](QNetworkReply* reply, QNetworkReply::NetworkError){
        qDebug() << "HttpManager::slotQuitMirror false, error info " << reply->errorString();
        emit signalQuitMirror(false);
    };

    m_httpRequest->post(url, body, config);
}

void HttpManager::startCheckConnectState(bool)
{
    m_timer.start();
}

void HttpManager::slotPair(const QString &devName, const QString &pinCode)
{
    QString url = "/api/pin";

    QJsonObject body;
    body.insert("name", devName);
    body.insert("pin", pinCode);

    HttpRequest::RequestConfig config;
    config.onSuccess = [this](QNetworkReply*, const QByteArray& response){
        qDebug() << "HttpManager::slotPair success, response " << response;
        QJsonObject json = QJsonDocument::fromJson(response).object();
        int success = false;
        if(json.contains("status") && true == json.value("status").toBool())
        {
            success = true;
        }
        emit signalPairResult(success);
    };
    config.onError = [this](QNetworkReply* reply, QNetworkReply::NetworkError error){
        qDebug() << "HttpManager::slotPair false, error info " << reply->errorString();
        emit signalInitUserComplete(false);
    };
    m_httpRequest->post(url, body, config);
}

void HttpManager::slotInitUserComplete(bool success)
{
    if(success)
    {
        checkPair();
    }
    else
    {
        RevoUtils::openSunshine();
        QTimer::singleShot(5000, [this]{
            initHttpUser();
        });
    }
}

void HttpManager::initHttpUser()
{
    if(m_initTimeout)
    {
        emit signalInitTimeout();
        return;
    }

    QString url = "/api/password";

    QJsonObject body;
    body.insert("currentUsername", SUNSHINE_WEB_DEFAULT_USERNAME);
    body.insert("currentPassword", SUNSHINE_WEB_DEFAULT_PASSWORD);
    body.insert("newUsername", SUNSHINE_WEB_DEFAULT_USERNAME);
    body.insert("newPassword", SUNSHINE_WEB_DEFAULT_PASSWORD);
    body.insert("confirmNewPassword", SUNSHINE_WEB_DEFAULT_PASSWORD);

    HttpRequest::RequestConfig config;
    config.onSuccess = [this](QNetworkReply*, const QByteArray& response){
        qDebug() << "HttpManager::initHttpUser success, response " << response;
        emit signalInitUserComplete(true);
    };
    config.onError = [this](QNetworkReply* reply, QNetworkReply::NetworkError error){
        qDebug() << "HttpManager::initHttpUser false, error info " << reply->errorString();
        emit signalInitUserComplete(false);
    };

    m_httpRequest->post(url, body, config);
}

void HttpManager::checkPair()
{
    QString url = "/api/clients/list";

    HttpRequest::RequestConfig config;
    config.onSuccess = [this](QNetworkReply*, const QByteArray& response){
        qDebug() << "HttpManager::checkPair success, response " << response;
        QJsonObject json = QJsonDocument::fromJson(response).object();
        bool bPaired = false;
        if(json.end() != json.find("named_certs"))
        {
            QJsonArray array = json.value("named_certs").toArray();
            bPaired = !array.empty();
        }
        emit signalInitComplete(bPaired);
    };
    config.onError = [this](QNetworkReply* reply, QNetworkReply::NetworkError error){
        qDebug() << "HttpManager::checkPair false, error info " << reply->errorString();
        emit signalInitComplete(false);
    };

    m_httpRequest->get(url, config);
}

void HttpManager::checkConnectState()
{
    QString url = "/api/sessions/state";

    HttpRequest::RequestConfig config;
    config.onSuccess = [this](QNetworkReply*, const QByteArray& response){
        QJsonObject json = QJsonDocument::fromJson(response).object();
        int state = static_cast<int>(ConnectState::DISCONNECT);
        if(json.contains("state") && 3 == json.value("state").toInt())
        {
            state = static_cast<int>(ConnectState::CONNECT);
        }
        emit signalConnectState(state);
    };
    config.onError = [this](QNetworkReply* reply, QNetworkReply::NetworkError error){
        emit signalConnectState(false);
    };
    m_httpRequest->get(url, config);
}

QString HttpManager::getAuthorization()
{
    QString username = SUNSHINE_WEB_DEFAULT_USERNAME;
    QString password = SUNSHINE_WEB_DEFAULT_PASSWORD;
    QString credentials = username + ":" + password;
    QByteArray credentialsBase64 = credentials.toUtf8().toBase64();
    return "Basic " + credentialsBase64;
}
