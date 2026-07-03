#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

#include <atomic>
#include <memory>

#include "src/common.h"
#include "src/bl/httpmanager.h"

class LoadingDialog;
class MirrorPair;
class HighDpiHelper;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(bool bPaired READ isPaired WRITE setPaired NOTIFY bPairedChanged)
    Q_PROPERTY(int connectState READ connectState WRITE setConnectState NOTIFY connectStateChanged)

    enum class MirrorPage: int
    {
        INVALID,
        PAIR,
        RUNNING,
        STOPPED,
    };
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void closeEvent(QCloseEvent *event) override;

    void init(const QString& wifiName, const QString& wifiPwd);

    bool isPaired() const;
    void setPaired(bool newBPaired);

    int connectState() const;
    void setConnectState(int newConnectState);

    void setVisible(bool visible) override;

    void reopenWindow();
signals:
    void bPairedChanged();

    void connectStateChanged();

public slots:
    void slotHttpInitComplete(bool bPaired);
    void slotInitTimeout();

    void slotMirrorStopped();
    void slotMirrorRestart();

    void slotConnectState(int state);

    void slotConnectStateChanged();

    void slotStopMirrorComplete(bool success);

    void slotOpenMoreDialog();

    void slotQuitMirror(bool success);

    void slotCheckRevoVersion(const QString &url);
private:

    void initUi();

    void initHttp();

    void openLoadingDialog();
private:
    QStackedWidget* m_stackedWidget{ nullptr };

    MirrorPair* m_pairPage{ nullptr };
    LoadingDialog* m_loadingDialg{ nullptr };

    WifiInfo m_wifiInfo;

    bool m_svcClose{false}; // sunshine服务是否关闭

    std::atomic_bool m_bPaired{false};

    std::unique_ptr<HttpManager> m_httpManager{nullptr};
    int m_connectState{static_cast<int>(ConnectState::DISCONNECT)};

    HighDpiHelper* m_highDpiHelper{nullptr};;
};
#endif // MAINWINDOW_H
