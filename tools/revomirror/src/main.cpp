#include <QApplication>
#include <QFontDatabase>
#include <QProcess>
#include <QDebug>
#include <QDir>
#include <QRegularExpression>

#include "ui/mainwindow.h"
#include "util/singleapplication.h"
#include "util/simplelogger.h"
#include "util/configmanager.h"
#include "util/revoutils.h"
#include "ui/lang/languagemgr.h"

void loadFont()
{
    // 字体文件路径列表
    QStringList fontPaths = {
        ":/resources/fonts/SourceHanSansCN-Regular.ttf",
    };

    QString loadedFontFamily;

    for (const QString& fontPath : fontPaths) {
        int fontId = QFontDatabase::addApplicationFont(fontPath);

        if (fontId != -1) {
            QStringList families = QFontDatabase::applicationFontFamilies(fontId);

            if (!families.isEmpty()) {
                loadedFontFamily = families.first();
                qDebug() << "成功加载字体:" << loadedFontFamily;
            }
        }
    }
}

bool startSunshine()
{
    QString path = "C:/Program Files/Sunshine/sunshine.exe";
    qDebug() << "path: " << QCoreApplication::applicationDirPath();

    QProcess sunshine;
    sunshine.setProgram(path);
    sunshine.setArguments(QStringList() << "--shortcut");

    sunshine.start();

    if(!sunshine.waitForStarted())
    {
        qDebug() << "sunshine start failed!";
        return false;
    }
    return true;
}

QString loadStyleSheet()
{
    // 从资源文件加载样式表
    QString fileName = ":/resources/styles/style.qss";
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "can't open stylesheet file:" << fileName;
        return QString();
    }

    QTextStream stream(&file);
    QString styleSheet = stream.readAll();
    file.close();

    return styleSheet;
}

QString getAppDir()
{
#if defined (__APPLE__) || defined(__MACH__)
  // 可执行文件的路径格式是 /Sunshine.app/Contents/MacOS/sunshine，这里要获取.app所在目录，所以要向上退三层
  QDir dir = QDir(QCoreApplication::applicationDirPath());
  dir.cdUp();
  dir.cdUp();
  dir.cdUp();
  return dir.absolutePath();
#else
  return QCoreApplication::applicationDirPath();
#endif
}

int main(int argc, char *argv[])
{
    // 1. 设置缩放，支持按小数缩放
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    SingleApplication app(argc, argv, QString("RevoMirror_%1").arg(PROJECT_VERSION));

    // 检查是否已有实例在运行
    if (app.isRunning()) {
        // 发送消息给已运行的实例
        app.sendMessage("show");

        return 0; // 退出当前实例
    }

    // 一行代码初始化日志
    SimpleLogger::initialize("RevoMirror.log");

    qInfo() << "log info path is:" << SimpleLogger::getCurrentLogFile();

    qInfo() << "--------------------------- RevoMirror START ---------------------------";

    qInfo() << "version " << PROJECT_VERSION;



//     QProcess sunshine;

// #ifdef __APPLE__
//     //QString path = "/Users/mima0000/RevoMirror/build/package/Sunshine.app";
//     QString path = getAppDir() + "/Sunshine.app";
//     qInfo() << "sunshine path: " << path;

//     QStringList openArgs;
//     openArgs << "-a" << path;  // -a 参数指定应用程序

//     sunshine.start("open", openArgs);
//   #else
//     QString path = "C:/Program Files/Revo Mirror/sunshine.exe";
//     //QString path = QCoreApplication::applicationDirPath() + "/sunshine.exe";

//     qInfo() << "sunshine path: " << path;

//     sunshine.setProgram(path);
//     sunshine.setArguments(QStringList() << "--shortcut");
//     sunshine.start();

//   #endif


//     if(!sunshine.waitForStarted())
//     {
//         QProcess::ProcessError error = sunshine.error();
//         QString errorString = sunshine.errorString();
//         qWarning() << "sunshine start failed! Error code:" << error << "Error string:" << errorString;
//         qInfo() << "--------------------------- RevoMirror End ---------------------------";
//         return 0;
//     }
//     else
//     {
//         qInfo() << "sunshine start success!";
//     }

    RevoUtils::openSunshine();


    // 在 main 函数或初始化时添加
    qDebug() << "SSL support:" << QSslSocket::supportsSsl();
    qDebug() << "SSL library version:" << QSslSocket::sslLibraryVersionString();

    QString styleSheet = loadStyleSheet();
    if (!styleSheet.isEmpty()) {
        app.setStyleSheet(styleSheet);
    }

    LanguageMgr::switchLanguage(ConfigManager::instance().getLanguage());

    // 正则表达式，匹配 -WIFI-NAME="xxx"
    QRegularExpression nameRegex(R"(WIFI-NAME=([^"]+))");
    QRegularExpression pswdRegex(R"(WIFI-PSWD=([^"]+))");

    QString wifiName, wifiPswd;
    QStringList arguments = QApplication::arguments();
    qDebug() << "arguments: " << arguments;
    for (const QString &arg : arguments) {
        QRegularExpressionMatch nameMatch = nameRegex.match(arg);
        if (nameMatch.hasMatch()) {
            wifiName = nameMatch.captured(1);
            continue;
        }
        QRegularExpressionMatch pwdMatch = pswdRegex.match(arg);
        if (pwdMatch.hasMatch()) {
            wifiPswd = pwdMatch.captured(1);
            continue;
        }
    }

    loadFont();

    MainWindow window;
    // 设置主窗口到单实例管理器
    app.setMainWidget(&window);
    // 连接信号
    QObject::connect(&app, &SingleApplication::instanceStarted, [&](){
        // 当有新实例尝试启动时的处理
        qInfo() << "SingleApplication::instanceStarted";
        window.reopenWindow();
    });

    QObject::connect(&app, &SingleApplication::messageReceived, [&](const QString &message){
        // 处理接收到的消息
        if (message == "show") {
            qInfo() << "SingleApplication::messageReceived";
            window.reopenWindow();
        }
        // 可以根据不同消息执行不同操作
    });

    window.init(wifiName, wifiPswd);
    int ret = app.exec();

    qInfo() << "--------------------------- RevoMirror End ---------------------------";
    return ret;
}
