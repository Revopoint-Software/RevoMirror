#include "httprequest.h"

#include <QNetworkRequest>
#include <QTimer>
#include <QJsonDocument>
#include <QUrl>
#include <QDebug>

HttpRequest::HttpRequest(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_defaultTimeout(30000)
{
    // 设置默认请求头
    m_defaultHeaders["Accept"] = "*/*";
    m_defaultHeaders["Connection"] = "keep-alive";
    m_defaultHeaders["Content-Type"] = "application/json";
    m_defaultHeaders["Accept-Encoding"] = "gzip, deflate, br";
    m_defaultHeaders["Accept-Language"] = "zh-CN";
}

HttpRequest::~HttpRequest()
{
}

void HttpRequest::setServerAddress(const QString &address)
{
    m_serverAddres = address;
}

void HttpRequest::setSSLEnable(bool enable)
{
    m_enableSSL = enable;
}

void HttpRequest::setAuthorization(const QString &value)
{
    m_authorization = value;
}

void HttpRequest::get(const QString &url, const RequestConfig &config)
{
    emit requestStarted(url);

    QNetworkRequest request = createRequest(url, config);
    QNetworkReply *reply = m_networkManager->get(request);
    setupReplyHandlers(reply, config);
}

void HttpRequest::post(const QString &url, const QJsonObject &json, const RequestConfig &config)
{
    emit requestStarted(url);

    QNetworkRequest request = createRequest(url, config);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (m_networkManager->networkAccessible() == QNetworkAccessManager::NotAccessible) {
        m_networkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
    }
#endif

    QNetworkReply *reply = m_networkManager->post(request, data);
    setupReplyHandlers(reply, config);
}

void HttpRequest::put(const QString &url, const QJsonObject &json, const RequestConfig &config)
{
    emit requestStarted(url);

    QNetworkRequest request = createRequest(url, config);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    QNetworkReply *reply = m_networkManager->put(request, data);
    setupReplyHandlers(reply, config);
}

void HttpRequest::deleteResource(const QString &url, const RequestConfig &config)
{
    emit requestStarted(url);

    QNetworkRequest request = createRequest(url, config);
    QNetworkReply *reply = m_networkManager->deleteResource(request);
    setupReplyHandlers(reply, config);
}

void HttpRequest::setDefaultHeader(const QString &key, const QString &value)
{
    m_defaultHeaders[key] = value;
}

void HttpRequest::removeDefaultHeader(const QString &key)
{
    m_defaultHeaders.remove(key);
}

void HttpRequest::clearDefaultHeaders()
{
    m_defaultHeaders.clear();
}

void HttpRequest::setDefaultTimeout(int milliseconds)
{
    m_defaultTimeout = milliseconds;
}

QNetworkRequest HttpRequest::createRequest(const QString &url, const RequestConfig &config)
{
    QNetworkRequest request{QUrl(m_serverAddres + url)};

    // 添加这部分：为 localhost 配置 SSL
    if (m_enableSSL) {
        QSslConfiguration sslConfig = request.sslConfiguration();
        sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
        request.setSslConfiguration(sslConfig);
    }

    if(!m_authorization.isEmpty())
    {
        request.setRawHeader("Authorization", m_authorization.toLatin1());
    }

    // 设置默认请求头
    for (auto it = m_defaultHeaders.begin(); it != m_defaultHeaders.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    // 设置自定义请求头（会覆盖默认值）
    for (auto it = config.headers.begin(); it != config.headers.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    // 设置其他属性
    //request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    //request.setTransferTimeout(config.timeout > 0 ? config.timeout : m_defaultTimeout);

    return request;
}

void HttpRequest::setupReplyHandlers(QNetworkReply *reply, const RequestConfig &config)
{
    // 处理完成信号
    connect(reply, &QNetworkReply::finished, this, [this, reply, config]() {

        const QString url = reply->url().toString();

        if (reply->error() == QNetworkReply::NoError) {
            //qDebug() << "http request success, " << reply->url().toString() << ", response " << reply->readAll();

            QByteArray responseData = reply->readAll();
            emit requestFinished(url);

            if (config.onSuccess) {
                config.onSuccess(reply, responseData);
            }
        } else {
            //qDebug() << "http request error, " << reply->url().toString() << "; error info " << reply->errorString();

            QString errorString = reply->errorString();
            emit requestError(url, errorString);

            if (config.onError) {
                config.onError(reply, reply->error());
            }
        }

        reply->deleteLater();
    });

    // 处理超时
    QTimer *timer = new QTimer(reply);
    timer->setSingleShot(true);
    timer->setInterval(config.timeout > 0 ? config.timeout : m_defaultTimeout);

    connect(timer, &QTimer::timeout, this, [this, reply, config]() {
        //qDebug() << "http request timemout, " << reply->url().toString();
        if (reply->isRunning()) {
            reply->abort();
            const QString url = reply->url().toString();
            emit requestError(url, "Request timeout");

            if (config.onError) {
                config.onError(reply, QNetworkReply::TimeoutError);
            }
        }
    });

    timer->start();
}

