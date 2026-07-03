#include "versionmanager.h"

#include <QEventLoop>
#include <QTimer>
#include <QDebug>
#include <QJsonObject>
#include <QUrl>

#include "src/ui/lang/languagemgr.h"
#include "src/common.h"

#ifdef Q_OS_WIN
#define REVO_SN "FFFFFFFFFFFFFR011"
#else
#define REVO_SN "FFFFFFFFFFFFFR012"
#endif

VersionManager::VersionManager(QObject *parent)
    : QObject(parent)
    , m_downloadManager(new HttpRequest())
{
    m_downloadManager->setSSLEnable(false);
    m_currentVersion = QString("v%1").arg(PROJECT_VERSION);
}

void VersionManager::syncCheckLatestVersion()
{
    checkLatestVersion();

    QEventLoop loop;
    connect(this, &VersionManager::sigReplyFinish, &loop, &QEventLoop::quit);

    // 启动定时器，在超时后手动结束事件循环
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);
    const int timeoutMilliseconds = 15000;
    QObject::connect(&timeoutTimer, &QTimer::timeout, [&loop]() {
        qInfo() << "syncCheckLatestVersion Timeout occurred.";
        loop.quit();
    });
    qInfo() << "timer start";
    timeoutTimer.start(timeoutMilliseconds);

    loop.exec();

    if (timeoutTimer.isActive()) {
        qInfo() << "timer stop";
        timeoutTimer.stop();
    }
}

void VersionManager::asyncCheckLatestVersion()
{
    checkLatestVersion();
}

VersionManager &VersionManager::instance()
{
    static VersionManager _instance;
    return _instance;
}

void VersionManager::checkLatestVersion()
{
    QString url; // = RevoScanConfig::getRevoTestServer();
    if (url.isEmpty()) {
        url = SOFTWARE_SERVER_LINK;
    }

    auto lang = LanguageMgr::getCurrentPrefix();
    if (lang == "ja") {
        lang = "ja_JP";
    } else if (lang == "pt_BR") {
        lang = "en_US";
    }

    QJsonObject body;
    body.insert("sn_code", REVO_SN);
    body.insert("deploy", "test");
    body.insert("user_version_string", m_currentVersion);
    body.insert("version_desc_language", lang);


    HttpRequest::RequestConfig config;
    config.onSuccess = [this](QNetworkReply* postReply, const QByteArray& response){
        qDebug() << "VersionManager::checkLatestVersion success, response " << response;
//        QJsonObject json = QJsonDocument::fromJson(response).object();
//        slotPostReply(json);
//        emit sigReplyFinish();




        QJsonObject result;

        int statusCode = postReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if((statusCode == 200) || (statusCode == 302))
        {
            result = QJsonDocument::fromJson(response).object();
        }

//        m_postReply->deleteLater();
//        m_postReply = nullptr;

        slotPostReply(result);
        emit sigReplyFinish();
    };
    config.onError = [this](QNetworkReply* reply, QNetworkReply::NetworkError){
        qDebug() << "VersionManager::checkLatestVersion false, error info " << reply->errorString();
        slotPostReply(QJsonObject());
        emit sigReplyFinish();
    };


    m_downloadManager->post(url, body, config);
}

void VersionManager::slotPostReply(const QJsonObject &versionInfo)
{
    m_checkVersionResult = false;

    if (!versionInfo.contains("code") || versionInfo["code"].toInt() != 200) {
        if (versionInfo["code"].toInt() == 1000) {
            m_latestVersion = m_currentVersion;
            m_checkVersionResult = true;
            qInfo() << "is newest revo mirror version";
        } else {
            qInfo() << "post server fail";
        }
        return;
    }
    auto iterData = versionInfo.find("data");
    if (iterData == versionInfo.end()) {
        qInfo() << "parse data fail";
        return;
    }
    auto dataObject = iterData->toObject();
    if (dataObject.isEmpty() || !dataObject.contains("target_version") || !dataObject.contains("ota_url")) {
        qInfo() << "parse data content fail";
        return;
    }
    QString targetVersion = dataObject["target_version"].toString();//(*iterData)["target_version"].toString();
    QString otaUrl = dataObject["ota_url"].toString();//(*iterData)["ota_url"].toString();

    m_versionDesc = dataObject["version_desc"].toString();//(*iterData)["version_desc"].toString();
    m_downloadUrl = otaUrl;
    m_latestVersion = targetVersion;

    qInfo() << "check upgrade success";
    m_checkVersionResult = true;
    emit sigLatestVersion(otaUrl);
}

QString VersionManager::currentVersion() const
{
    return m_currentVersion;
}

QString VersionManager::latestVersion() const
{
    return m_latestVersion;
}

QString VersionManager::downloadUrl() const
{
    return m_downloadUrl;
}

QString VersionManager::versionDesc() const
{
    return m_versionDesc;
}

bool VersionManager::isLatestVersion() const
{
    return m_currentVersion == m_latestVersion;
}

int VersionManager::checkTimes() const
{
    return m_checkTimes;
}

