#include "mainwindow.h"

#include <QFile>
#include <QLabel>
#include <QLayout>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSysInfo>
#include <QTextStream>
#include <QApplication>
#include <qDebug>

#include "src/ui/customtitlebar.h"
#include "src/ui/mirrorpair.h"
#include "src/ui/mirrorrunning.h"
#include "src/ui/mirrorstopped.h"
#include "src/ui/moredialog.h"
#include "src/ui/loadingdialog.h"
#include "src/ui/versiondialog.h"

#include "src/bl/versionmanager.h"
#include "src/bl/httpmanager.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_httpManager(std::make_unique<HttpManager>())
{
    connect(m_httpManager.get(), &HttpManager::signalInitComplete, this, &MainWindow::slotHttpInitComplete);
    connect(m_httpManager.get(), &HttpManager::signalConnectState, this, &MainWindow::slotConnectState);
    connect(&VersionManager::instance(), &VersionManager::sigLatestVersion, this, &MainWindow::slotCheckRevoVersion);

    connect(this, &MainWindow::connectStateChanged, this, &MainWindow::slotConnectStateChanged);

    // 隐藏系统标题栏
    setWindowFlags(Qt::FramelessWindowHint);
}

MainWindow::~MainWindow()
{
    m_pairPage = nullptr;
}

void MainWindow::init(const QString& wifiName, const QString& wifiPwd)
{

    m_wifiInfo.name = wifiName;
    m_wifiInfo.password = wifiPwd;

    initHttp();
    initUi();

    hide();
    setVisible(false);

    QTimer::singleShot(100, this, &MainWindow::openLoadingDialog);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qInfo() << "MainWindow::closeEvent " << m_svcClose;
    if (m_svcClose) {
        event->accept();  // 接受关闭事件
        // 确保应用程序退出
        QApplication::quit();
    } else {
        event->ignore();
        hide();
        QTimer::singleShot(500, [this](){
            m_svcClose = true;
            close();
        });
        m_httpManager->quitMirrorService();
    }
}

void MainWindow::initUi()
{
    resize(WINDOW_W, WINDOW_H);

    // 添加自定义标题栏
    CustomTitleBar* titleBar = new CustomTitleBar(this);


    m_stackedWidget = new QStackedWidget(this);
    m_stackedWidget->setGeometry(0, TITILE_BAR_H, CLICENT_W, CLICENT_H);

    QWidget* invalidPage = new QWidget();
    invalidPage->resize(CLICENT_W, CLICENT_H);
    m_stackedWidget->addWidget(invalidPage);

    m_pairPage = new MirrorPair();
    m_pairPage->init(m_wifiInfo);
    m_stackedWidget->addWidget(m_pairPage);

    MirrorRunning* runningPage = new MirrorRunning();
    m_stackedWidget->addWidget(runningPage);

    MirrorStopped* stoppedPage = new MirrorStopped();
    m_stackedWidget->addWidget(stoppedPage);


    connect(m_httpManager.get(), &HttpManager::signalQuitMirror, this, &MainWindow::slotQuitMirror);
    connect(titleBar, &CustomTitleBar::signalOpenMoreDialog, this, &MainWindow::slotOpenMoreDialog);

    connect(m_pairPage, &MirrorPair::signalPairStart, m_httpManager.get(), &HttpManager::slotPair);
    connect(m_httpManager.get(), &HttpManager::signalPairResult, m_pairPage, &MirrorPair::slotPairComplete);

    connect(m_httpManager.get(), &HttpManager::signalInitTimeout, this, &MainWindow::slotInitTimeout);

    connect(runningPage, &MirrorRunning::signalMirrorStopped, this, &MainWindow::slotMirrorStopped);
    connect(stoppedPage, &MirrorStopped::signalMirrorRestart, this, &MainWindow::slotMirrorRestart);

    m_stackedWidget->setCurrentIndex(static_cast<int>(MirrorPage::INVALID)); // 默认显示第一个页面
}

void MainWindow::slotHttpInitComplete(bool bPaired)
{
    qDebug() << "MainWindow::slotHttpInitComplete " << bPaired;

    setPaired(bPaired);
    m_pairPage->setPairState(bPaired ? static_cast<int>(PairState::PAIRED) : static_cast<int>(PairState::NONE));

    m_stackedWidget->setCurrentIndex(static_cast<int>(MirrorPage::PAIR));

    if(m_loadingDialg)
    {
        qDebug() << "destroy loadingDialg";
        m_loadingDialg->hide();
        delete m_loadingDialg;
        m_loadingDialg = nullptr;
    }

    show();

    QTimer::singleShot(500,[=]() {
        VersionManager::instance().asyncCheckLatestVersion();
    });
}

void MainWindow::slotInitTimeout()
{
    if(m_loadingDialg)
    {
        m_loadingDialg->setLoadingFail();
    }

    QTimer::singleShot(3000, [this]{
        m_svcClose = true;
        close();
    });
}

void MainWindow::slotMirrorStopped()
{
    m_httpManager->stopMirror();
}

void MainWindow::slotMirrorRestart()
{
    m_pairPage->setPairState(static_cast<int>(PairState::PAIRED));

    m_stackedWidget->setCurrentIndex(static_cast<int>(MirrorPage::PAIR));
}

void MainWindow::slotConnectState(int state)
{
    setConnectState(state);
}

void MainWindow::slotConnectStateChanged()
{
    if(static_cast<int>(ConnectState::DISCONNECT) == connectState())
    {
        m_stackedWidget->setCurrentIndex(static_cast<int>(MirrorPage::STOPPED));
    }
    else if(static_cast<int>(ConnectState::CONNECT) == connectState())
    {
        m_stackedWidget->setCurrentIndex(static_cast<int>(MirrorPage::RUNNING));
    }
}

void MainWindow::slotStopMirrorComplete(bool success)
{
    setConnectState(static_cast<int>(MirrorPage::STOPPED));
}

void MainWindow::slotOpenMoreDialog()
{
    MoreDialog dialog(this);
    dialog.exec();
}

void MainWindow::slotQuitMirror(bool success)
{
    m_svcClose = true;
    close();
}

void MainWindow::slotCheckRevoVersion(const QString &url)
{
    if(url.isEmpty())
    {
        qWarning() << "MainWindow::slotCheckRevoVersion url is empty";
        return;
    }
    VersionConfig verConfig;
    verConfig.manual = false;
    verConfig.url = VersionManager::instance().downloadUrl();
    verConfig.description = VersionManager::instance().versionDesc();
    verConfig.version = VersionManager::instance().latestVersion();
    VersionDialog versionDialog(verConfig, this);
    versionDialog.exec();
}

void MainWindow::initHttp()
{
    m_httpManager->init();
}

void MainWindow::openLoadingDialog()
{
    qDebug() << "MainWindow::openLoadingDialog";

    // stackedWidget不是无效页面，说明和sunshine的通信已经完成，不需要在显示加载中页面，所以这里直接返回。
    if(static_cast<int>(MirrorPage::INVALID) != m_stackedWidget->currentIndex())
    {
        return;
    }

    hide();
    m_loadingDialg = new LoadingDialog(this);
    m_loadingDialg->exec();
}

bool MainWindow::isPaired() const
{
    return m_bPaired;
}

void MainWindow::setPaired(bool newBPaired)
{
    if (m_bPaired == newBPaired)
        return;
    m_bPaired = newBPaired;
    emit bPairedChanged();
}

int MainWindow::connectState() const
{
    return m_connectState;
}

void MainWindow::setConnectState(int newConnectState)
{
    if (m_connectState == newConnectState)
        return;
    m_connectState = newConnectState;
    emit connectStateChanged();
}

void MainWindow::setVisible(bool visible)
{
    qDebug() << "MainWindow::setVisible" << visible;
    if (visible) {
       if(m_loadingDialg && m_loadingDialg->isVisible())
       {
           return;
       }
    }
    QMainWindow::setVisible(visible);
}

void MainWindow::reopenWindow()
{
    // 采用先最小化再恢复（强制系统重新关注）
    if(m_loadingDialg && m_loadingDialg->isVisible())
    {
        m_loadingDialg->showMinimized();
        m_loadingDialg->showNormal();
        return;
    }
    showMinimized();
    showNormal();
}
