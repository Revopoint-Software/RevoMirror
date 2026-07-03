#ifndef HTTPMANAGER_H
#define HTTPMANAGER_H

#include <QObject>
#include <QThread>
#include <QTimer>

#include <atomic>
#include <memory>

#include "src/comm/httprequest.h"
#include "src/common.h"


class HttpManager : public QObject
{
    Q_OBJECT
public:
    explicit HttpManager(QObject *parent = nullptr);
    ~HttpManager();

    void init();

    void stopMirror();

    void startCheckConnectState(bool);

    void quitMirrorService();

public slots:
    void slotInitUserComplete(bool success);
    void slotPair(const QString& devName, const QString& pinCode);

signals:
    void signalInitUserComplete(bool success);
    void signalInitTimeout();
    void signalInitComplete(bool bPaired);
    void signalConnectState(int state);

    void signalStopMirrorComplete(bool success);

    void signalPairResult(bool success);

    void signalQuitMirror(bool success);
private:
    void initHttpUser();

    void checkPair();

    void checkConnectState();

    QString getAuthorization();

private:
    std::unique_ptr<HttpRequest> m_httpRequest{nullptr};

    QTimer m_timer;

    bool m_initTimeout{false};
};

#endif // HTTPMANAGER_H
