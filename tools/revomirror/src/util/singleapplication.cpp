// singleapplication.cpp
#include "singleapplication.h"
#include <QLocalSocket>
#include <QFileInfo>
#include <QDir>
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QDebug>

SingleApplication::SingleApplication(int &argc, char **argv, const QString &uniqueKey)
    : QApplication(argc, argv)
    , _isRunning(false)
    , _uniqueKey(uniqueKey)
    , _localServer(nullptr)
    , _mainWidget(nullptr)
{
  // 创建基于应用程序路径的唯一标识
    QString appName = _uniqueKey;
    if (appName.isEmpty()) {
        appName = QFileInfo(QCoreApplication::applicationFilePath()).baseName();
    }
    // 创建更安全的服务器名称
    QString serverName;
  #ifdef Q_OS_MAC
    // Mac平台使用用户特定的路径
    QString userPath = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation);
    if (userPath.isEmpty()) {
        userPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    }
    serverName = QString("%1/%2").arg(userPath, appName);
  #else
    serverName = appName;
  #endif
    // 使用 MD5 确保服务器名称的唯一性和合法性
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(serverName.toUtf8());
    _serverName = hash.result().toHex();
    _sharedMemory.setKey(_serverName);
    if (_sharedMemory.attach()) {
        _isRunning = true;
    } else {
        _isRunning = false;
        // 创建共享内存
        if (!_sharedMemory.create(1)) {
            qDebug() << "Unable to create shared memory.";
            return;
        }
        // 清理可能存在的服务器
        QLocalServer::removeServer(_serverName);
        // 创建本地服务器
        _localServer = new QLocalServer(this);
        connect(_localServer, &QLocalServer::newConnection,
                this, &SingleApplication::receiveConnection);
        if (!_localServer->listen(_serverName)) {
            qDebug() << "Failed to start server:" << _localServer->errorString();
            qDebug() << "Server name:" << _serverName;
        } else {
            qDebug() << "Server started successfully:" << _serverName;
        }
    }
}

SingleApplication::~SingleApplication()
{
    if (_localServer) {
        _localServer->close();
        delete _localServer;
        _localServer = nullptr;
    }
    
    // 清理共享内存
    if (_sharedMemory.isAttached()) {
        _sharedMemory.detach();
    }
}

bool SingleApplication::isRunning()
{
    return _isRunning;
}

bool SingleApplication::sendMessage(const QString &message)
{
    if (!_isRunning) {
        return false;
    }

    QLocalSocket localSocket(this);
    localSocket.connectToServer(_serverName, QIODevice::WriteOnly);

    if (!localSocket.waitForConnected(TIMEOUT)) {
        qDebug() << "Failed to connect to local server:" << localSocket.errorString();
        qDebug() << "Server name:" << _serverName;
        return false;
    }

    localSocket.write(message.toUtf8());
    if (!localSocket.waitForBytesWritten(TIMEOUT)) {
        qDebug() << "Failed to write message:" << localSocket.errorString();
        return false;
    }

    localSocket.disconnectFromServer();
    return true;
}

void SingleApplication::setMainWidget(QWidget *widget)
{
    _mainWidget = widget;
}

void SingleApplication::receiveConnection()
{
    QLocalSocket *localSocket = _localServer->nextPendingConnection();
    if (!localSocket) {
        return;
    }

    connect(localSocket, &QLocalSocket::readyRead, this, &SingleApplication::receiveMessage);
    connect(localSocket, &QLocalSocket::disconnected, localSocket, &QLocalSocket::deleteLater);

    // 发出实例启动信号
    emit instanceStarted();
}

void SingleApplication::receiveMessage()
{
    QLocalSocket *localSocket = qobject_cast<QLocalSocket*>(sender());
    if (!localSocket) {
        return;
    }

    QByteArray byteArray = localSocket->readAll();
    QString message = QString::fromUtf8(byteArray.constData());

    emit messageReceived(message);

    // 置顶主窗口
    if (_mainWidget) {
        _mainWidget->show();
        _mainWidget->raise();
        _mainWidget->activateWindow();

        // Windows 特殊处理
#ifdef Q_OS_WIN
        _mainWidget->setWindowState((_mainWidget->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
#endif
    }
}
