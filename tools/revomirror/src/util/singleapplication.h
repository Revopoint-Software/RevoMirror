// singleapplication.h
#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QApplication>
#include <QSharedMemory>
#include <QLocalServer>
#include <QLocalSocket>
#include <QWidget>

class SingleApplication : public QApplication
{
    Q_OBJECT

public:
    SingleApplication(int &argc, char **argv, const QString &uniqueKey);
    ~SingleApplication();

    bool isRunning();
    bool sendMessage(const QString &message);
    void setMainWidget(QWidget *widget);

public slots:
    void receiveConnection();
    void receiveMessage();

signals:
    void messageReceived(const QString &message);
    void instanceStarted();

private:
    bool _isRunning;
    QString _uniqueKey;
    QSharedMemory _sharedMemory;
    QLocalServer *_localServer;
    QWidget *_mainWidget;
    QString _serverName;  // 添加这个成员变量ß

    static const int TIMEOUT = 1000;
};

#endif // SINGLEAPPLICATION_H
