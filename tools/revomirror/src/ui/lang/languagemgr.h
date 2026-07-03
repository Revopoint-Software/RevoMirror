#ifndef LANGUAGEMGR_H
#define LANGUAGEMGR_H

#include <QMap>
#include <QString>
#include <QCoreApplication>
#include <QDir>

//QString getconst QString& path=QDir(QCoreApplication::applicationDirPath()).cdUp().absolutePath() + QDir::separator() + "Resources" + QDir::separator() +"lang"))

class LanguageMgr
{
public:
    LanguageMgr();

#if defined (__APPLE__) || defined(__MACH__)
    static QString getMacLanguagePath();

    static QMap<QString, QString> getLanguage(const QString& path=getMacLanguagePath(), const QString & prefix="language");
    static void switchLanguage(const QString& locale, const QString & prefix="language", const QString & path=getMacLanguagePath());
#else
    static QMap<QString, QString> getLanguage(const QString& path=QCoreApplication::applicationDirPath() + QDir::separator() +"lang", const QString & prefix="language");
    static void switchLanguage(const QString& locale, const QString & prefix="language", const QString & path=QCoreApplication::applicationDirPath() + QDir::separator() +"lang");
#endif
    static QString getCurrentPrefix(); 
    static QString getCurrentLanguageName();

private:
    static QString g_languageCurrentLocale;
//    static void retranslateUi(class QObject* parent);
};

#endif // LANGUAGEMGR_H
