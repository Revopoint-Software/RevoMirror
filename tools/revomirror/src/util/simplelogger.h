#ifndef SIMPLE_LOGGER_H
#define SIMPLE_LOGGER_H

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QStandardPaths>

class SimpleLogger {
public:
    // 初始化日志系统
    static void initialize(const QString& fileName = "app.log",
                          qint64 maxFileSize = 10 * 1024 * 1024,  // 默认10MB
                          int maxFileCount = 5) {                  // 默认保留5个文件
        static QMutex mutex;
        QMutexLocker locker(&mutex);

        if (!logFile) {
            baseFileName = fileName;
            maxSize = maxFileSize;
            maxCount = maxFileCount;


            QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
            // 创建logs目录
            logDir = appDataDir + "/logs";
            QDir().mkpath(logDir);

            // 创建初始日志文件
            createNewLogFile();

            // 安装消息处理器
            qInstallMessageHandler(messageHandler);

            // 清理旧文件
            cleanupOldFiles();
            
            // 显示日志文件信息
            //showLogFileInfo();
        }
    }

    // 获取当前日志文件路径
    static QString getCurrentLogFile() {
        return currentLogFile;
    }

    // 获取日志文件数量
    static int getLogFileCount() {
        return getExistingLogFiles().size();
    }

    // 获取所有日志文件列表
    static QStringList getLogFileList() {
        return getExistingLogFiles();
    }

private:
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);

        QString typeStr;
        switch (type) {
        case QtDebugMsg:    typeStr = "DEBUG"; break;
        case QtInfoMsg:     typeStr = "INFO "; break;
        case QtWarningMsg:  typeStr = "WARN "; break;
        case QtCriticalMsg: typeStr = "ERROR"; break;
        case QtFatalMsg:    typeStr = "FATAL"; break;
        }

        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

        // 处理分类信息
        QString category;
        if (context.category && QString(context.category) != "default") {
            category = QString("[%1] ").arg(context.category);
        }

        // 处理文件和行号信息
        QString location;
        if (context.file && context.line > 0) {
            QString fileName = QFileInfo(context.file).baseName();
            location = QString(" (%1:%2)").arg(fileName).arg(context.line);
        }

        QString formattedMsg = QString("[%1] %2 %3%4%5")
                              .arg(timestamp, typeStr, category, msg, location);

        // 输出到控制台
        printf("%s\n", formattedMsg.toLocal8Bit().constData());

        fflush(stdout);  // 强制刷新标准输出
        fflush(stderr);  // 强制刷新标准错误输出

        // 检查文件大小并轮转
        checkAndRotateFile();

        // 写入文件
        if (logStream) {
            *logStream << formattedMsg << "\r\n";
            logStream->flush();
        }
    }

    // 创建新的日志文件
    static void createNewLogFile() {
        // 关闭当前文件
        if (logStream) {
            delete logStream;
            logStream = nullptr;
        }
        if (logFile) {
            logFile->close();
            delete logFile;
            logFile = nullptr;
        }

        // 生成新文件名（带时间戳）
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
        QFileInfo info(baseFileName);
        QString newFileName = QString("%1_%2.%3")
                             .arg(info.baseName())
                             .arg(timestamp)
                             .arg(info.suffix().isEmpty() ? "log" : info.suffix());

        currentLogFile = QDir(logDir).absoluteFilePath(newFileName);

        // 创建新文件
        logFile = new QFile(currentLogFile);
        if (logFile->open(QIODevice::WriteOnly | QIODevice::Append)) {
            logStream = new QTextStream(logFile);
            //logStream->setEncoding(QStringConverter::Utf8);
        }
    }

    // 检查文件大小并轮转
    static void checkAndRotateFile() {
        if (!logFile) return;

        // 检查文件大小
        if (logFile->size() >= maxSize) {
            // 创建新文件
            createNewLogFile();

            // 清理旧文件
            cleanupOldFiles();

            // 显示轮转信息
            if (logStream) {
                QString rotateMsg = QString("=== checkAndRotateFile === current file: %1, files count: %2")
                                   .arg(QFileInfo(currentLogFile).fileName())
                                   .arg(getLogFileCount());
                printf("%s\n", rotateMsg.toLocal8Bit().constData());
            }
        }
    }

    // 清理旧文件，保持文件数量在限制内
    static void cleanupOldFiles() {
        QStringList logFiles = getExistingLogFiles();

        // 按修改时间排序（最新的在前）
        QList<QPair<QDateTime, QString>> fileList;
        for (const QString& file : logFiles) {
            QFileInfo info(file);
            fileList.append(qMakePair(info.lastModified(), file));
        }

        std::sort(fileList.begin(), fileList.end(),
                 [](const QPair<QDateTime, QString>& a, const QPair<QDateTime, QString>& b) {
                     return a.first > b.first; // 降序排列，最新的在前
                 });

        // 删除超出数量限制的文件
        while (fileList.size() > maxCount) {
            QString fileToDelete = fileList.takeLast().second;
            QFile::remove(fileToDelete);
            printf("删除旧日志文件: %s\n", QFileInfo(fileToDelete).fileName().toLocal8Bit().constData());
        }
    }

    // 获取现有的日志文件列表
    static QStringList getExistingLogFiles() {
        QDir dir(logDir);
        QFileInfo baseInfo(baseFileName);
        QString baseName = baseInfo.baseName();
        QString suffix = baseInfo.suffix().isEmpty() ? "log" : baseInfo.suffix();

        // 匹配模式：baseName_*.suffix
        QStringList nameFilters;
        nameFilters << QString("%1_*.%2").arg(baseName).arg(suffix);

        QStringList files = dir.entryList(nameFilters, QDir::Files, QDir::Time | QDir::Reversed);

        // 转换为完整路径
        QStringList fullPaths;
        for (const QString& file : files) {
            fullPaths << dir.absoluteFilePath(file);
        }

        return fullPaths;
    }

    // 显示日志文件信息
    static void showLogFileInfo() {
        QStringList logFiles = getExistingLogFiles();

        printf("=== 日志系统信息 ===\n");
        printf("日志目录: %s\n", QDir(logDir).absolutePath().toLocal8Bit().constData());
        printf("当前日志文件: %s\n", QFileInfo(currentLogFile).fileName().toLocal8Bit().constData());
        printf("最大文件大小: %.1f MB\n", maxSize / (1024.0 * 1024.0));
        printf("最大文件数量: %d\n", maxCount);
        printf("现有日志文件数量: %d\n", logFiles.size());

        if (!logFiles.isEmpty()) {
            printf("现有日志文件:\n");
            for (int i = 0; i < logFiles.size(); ++i) {
                QFileInfo info(logFiles[i]);
                printf("  %d. %s (%.1f KB, %s)\n",
                       i + 1,
                       info.fileName().toLocal8Bit().constData(),
                       info.size() / 1024.0,
                       info.lastModified().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().constData());
            }
        }
        printf("==================\n");
    }

public:
    // 手动轮转日志文件
    static void rotateNow() {
        static QMutex mutex;
        QMutexLocker locker(&mutex);

        if (logFile) {
            createNewLogFile();
            cleanupOldFiles();
            //showLogFileInfo();
        }
    }

    // 设置新的大小限制
    static void setMaxFileSize(qint64 size) {
        maxSize = size;
    }

    // 设置新的文件数量限制
    static void setMaxFileCount(int count) {
        maxCount = count;
    }

    // 获取当前文件大小
    static qint64 getCurrentFileSize() {
        return logFile ? logFile->size() : 0;
    }

    // 获取当前文件大小百分比
    static double getCurrentFileSizePercent() {
        if (!logFile || maxSize <= 0) return 0.0;
        return (double)logFile->size() / maxSize * 100.0;
    }

private:
    static QFile* logFile;
    static QTextStream* logStream;
    static QString baseFileName;
    static QString currentLogFile;
    static QString logDir;
    static qint64 maxSize;
    static int maxCount;
};

// 静态成员定义
QFile* SimpleLogger::logFile = nullptr;
QTextStream* SimpleLogger::logStream = nullptr;
QString SimpleLogger::baseFileName;
QString SimpleLogger::currentLogFile;
QString SimpleLogger::logDir;
qint64 SimpleLogger::maxSize = 10 * 1024 * 1024; // 10MB
int SimpleLogger::maxCount = 5;

#endif // SIMPLE_LOGGER_H
