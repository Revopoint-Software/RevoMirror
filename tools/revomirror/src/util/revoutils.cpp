#include "revoutils.h"

#include <QProcess>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QSettings>


namespace RevoUtils{
  const QString MAC_SUNSHINE_DEFAULT_PATH = "/Library/Services/Sunshine.app";
  // const QString MAC_SUNSHINE_DEFAULT_PATH = "/Users/gsf/renhl_test/revo_mirror/revo_mirror_merge/RevoMirror/build/Sunshine.app";
  // const QString MAC_SUNSHINE_DEFAULT_PATH = "/Users/gsf/renhl_test/revo_mirror/revo_mirror_merge/RevoMirror/build/package/Sunshine.app";
  const QString WIN_SUNSHINE_EXE_NAME = "sunshine.exe";

  QString getSunshineProgramPath()
  {
#if defined (__APPLE__) || defined(__MACH__)
    if(QFileInfo(MAC_SUNSHINE_DEFAULT_PATH).exists())
    {
      return MAC_SUNSHINE_DEFAULT_PATH;
    }

    // 1. 定义 /Volumes 根目录（macOS 所有挂载磁盘的根路径）
    const QString volumesRoot = "/Volumes";
    QDir volumesDir(volumesRoot);

    // 检查 /Volumes 目录是否存在（防御性判断）
    if (!volumesDir.exists()) {
      qCritical() << "错误：/Volumes 目录不存在，系统异常！";
      return "";
    }

    // 2. 遍历 /Volumes 下所有子项（只筛选目录，即挂载的磁盘）
    // 过滤规则：只保留目录，排除文件/符号链接
    QFileInfoList diskList = volumesDir.entryInfoList(
      QDir::Dirs | QDir::NoDotAndDotDot,  // 只列目录，排除 . 和 ..
      QDir::Name                          // 按名称排序
      );

    if (diskList.isEmpty()) {
      qInfo() << "⚠️ /Volumes 下无挂载的磁盘（仅系统盘）";
    }

    // 3. 遍历每个磁盘，检查/构造 Library/Services 路径
    for (const QFileInfo& diskInfo : diskList) {
      QString diskPath = diskInfo.absoluteFilePath();
      qInfo() << "\n=== 磁盘: " << diskPath << " ===";

      // 构造当前磁盘的 Library/Services 路径
      QString servicesPath = QString("%1/Library/Services/Sunshine.app").arg(diskPath);
      QDir servicesDir(servicesPath);

      // 检查路径是否存在
      if (servicesDir.exists()) {
        qInfo() << "✅ 存在: " << servicesPath;
        return servicesPath;
      }
    }
    qInfo() << "❌ 按路径未找到Sunshine.app";
    return "";
#endif
    QString path = QCoreApplication::applicationDirPath() + "\\" + WIN_SUNSHINE_EXE_NAME;
    if(QFile(path).exists())
    {
      return path;
    }
    QStringList registryPaths = {
      "HKEY_LOCAL_MACHINE\\SOFTWARE\\Revopoint"
    };

    for (const QString& basePath : registryPaths) {
      QSettings settings(basePath, QSettings::NativeFormat);
      QStringList groups = settings.childGroups();

      for (const QString& group : groups) {
        if ("Revo Mirror" == group)
        {
          QString installPath = settings.value("Revo Mirror/InstallPath").toString();
          path = installPath + "\\" + WIN_SUNSHINE_EXE_NAME;
          if (QFile(path).exists())
          {
            return path;
          }
          break;
        }
      }
    }
    return "";
  }

void openSunshine()
{
  QString path;
  QStringList args;
  bool bSuccess = false;

#ifdef __APPLE__
  // macOS 下启动 .app 需使用 open 命令，通过 -a 参数指定应用
  path = "open";
  // 应用路径（根据实际情况调整）
  //QString appPath = "/Users/mima0000/RevoMirror/build/package/Sunshine.app";
  //QString appPath = getAppDir() + "/Sunshine.app";
  //QString appPath = "/Library/Services/Sunshine.app";
  QString appPath = getSunshineProgramPath();
  if(appPath.isEmpty())
  {
    qWarning() << "can't find Sunshine.app";
    return;
  }
  // 构造参数：-a 指定应用路径，后续可添加传给应用的参数
  args << "-a" << appPath;
  qInfo() << "sunshine path: " << appPath;

          // 静态方法启动：程序路径为 "open"，参数为 args
  bSuccess = QProcess::startDetached(path, args);
#else
  // Windows 下直接启动 exe
  path = getSunshineProgramPath();
  if(path.isEmpty())
  {
    qWarning() << "can't find sunshine.exe";
    return;
  }
  // 传给 sunshine 的参数（如 --shortcut）
  args << "--shortcut";
  qInfo() << "sunshine path: " << path;

          // 静态方法启动：程序路径为 exe 路径，参数为 args
  bSuccess = QProcess::startDetached(path, args);
#endif

  if (!bSuccess)
  {
    qWarning() << "sunshine start failed!";
  }
  else
  {
    qInfo() << "sunshine start success!";
  }
}

};
