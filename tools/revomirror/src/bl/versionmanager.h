#ifndef VERSIONMANAGER_H
#define VERSIONMANAGER_H

#include <QObject>

#include "src/comm/httprequest.h"

class VersionManager : public QObject
{
    Q_OBJECT
public:
    explicit VersionManager(QObject *parent = nullptr);

    static VersionManager& instance();

    void syncCheckLatestVersion();

    void asyncCheckLatestVersion();

    bool checkVersionResult(){
        return m_checkVersionResult;
    };

    QString currentVersion() const;

    QString latestVersion() const;

    QString downloadUrl() const;

    QString versionDesc() const;

    bool isLatestVersion() const;

    int checkTimes() const;

signals:
    void sigLatestVersion(const QString &url);
    void sigReplyFinish();

private slots:
    void slotPostReply(const QJsonObject &versionInfo);

private:
    void checkLatestVersion();

private:
    QString m_currentVersion;
    QString m_latestVersion;
    QString m_downloadUrl;
    QString m_versionDesc;
    int m_checkTimes{ 0 };
    std::unique_ptr<HttpRequest> m_downloadManager{nullptr};
    bool m_checkVersionResult = false;

};

#endif // VERSIONMANAGER_H
