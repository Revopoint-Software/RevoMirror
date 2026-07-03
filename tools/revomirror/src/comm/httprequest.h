#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHash>
#include <memory>
#include <functional>

class HttpRequest : public QObject
{
    Q_OBJECT

public:
    // 响应回调类型
    using ResponseCallback = std::function<void(QNetworkReply*, const QByteArray&)>;
    using ErrorCallback = std::function<void(QNetworkReply*, QNetworkReply::NetworkError)>;

    // 请求配置
    struct RequestConfig {
        QHash<QString, QString> headers;
        int timeout = 30000; // 默认30秒超时
        ResponseCallback onSuccess = nullptr;
        ErrorCallback onError = nullptr;
    };

    explicit HttpRequest(QObject *parent = nullptr);
    ~HttpRequest();

    void setServerAddress(const QString& address);

    void setSSLEnable(bool enable);

    void setAuthorization(const QString& value);

    // 基础 HTTP 方法
    void get(const QString &url, const RequestConfig &config);
    void post(const QString &url, const QJsonObject &json, const RequestConfig &config);
    void put(const QString &url, const QJsonObject &json, const RequestConfig &config);
    void deleteResource(const QString &url, const RequestConfig &config);

    // 设置默认请求头
    void setDefaultHeader(const QString &key, const QString &value);
    void removeDefaultHeader(const QString &key);
    void clearDefaultHeaders();

    // 设置默认超时时间
    void setDefaultTimeout(int milliseconds);

signals:
    // 全局信号
    void requestStarted(const QString &url);
    void requestFinished(const QString &url);
    void requestError(const QString &url, const QString &error);

private:
    QNetworkAccessManager *m_networkManager;
    QHash<QString, QString> m_defaultHeaders;
    int m_defaultTimeout;
    QString m_serverAddres;
    bool m_enableSSL{false};

    QString m_authorization;

    // 内部方法
    QNetworkRequest createRequest(const QString &url, const RequestConfig &config);
    void handleReply(QNetworkReply *reply, const RequestConfig &config);
    void setupReplyHandlers(QNetworkReply *reply, const RequestConfig &config);

};

#endif // HTTPREQUEST_H
