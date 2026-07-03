#include "languagemgr.h"

#include <QDir>
#include <QLocale>
#include <QTranslator>
#include <QDebug>
#include <QList>
#include <QAction>
#include <QToolBar>
#include <QDockWidget>
#include <QMenu>
#include <QCoreApplication>
#include <QLibraryInfo>
#include <QLabel>
#include <QPushButton>

#define LANG_AUTO_TEXT   "Auto"

QString LanguageMgr::g_languageCurrentLocale;

LanguageMgr::LanguageMgr()
{

}


#if defined (__APPLE__) || defined(__MACH__)
QString LanguageMgr::getMacLanguagePath()
{
  QDir dir = QDir(QCoreApplication::applicationDirPath());
  dir.cdUp();
  return dir.absolutePath() + QDir::separator() + "Resources" + QDir::separator() +"lang";
}

#endif

QMap<QString, QString> LanguageMgr::getLanguage(const QString& path, const QString & prefix)
{
    QMap<QString, QString> res;
    // QLocale l;
    // qDebug() << l.name();
    res.insert(LANG_AUTO_TEXT, LANG_AUTO_TEXT);
    QDir qmDir(path);
    QStringList fileNames = qmDir.entryList(QStringList(prefix + "_*.qm"));
    for (int i = 0; i < fileNames.size(); ++i) {
        QString locale = fileNames[i];
        locale.remove(0, locale.indexOf('_') + 1);
        locale.chop(3);

        QLocale c(locale);
        QString title ="";
        if(QLocale::English == c.language()){
            title += QLocale::languageToString(c.language());
        }
        else{
            title += c.nativeLanguageName();
        }
            //    qDebug() << i <<locale << c.nativeLanguageName() << c.nativeCountryName() << QLocale::languageToString(c.language()) << c.uiLanguages() << c.countryToString(c.country());
        res.insert(locale, title);

    }
    return res;
}

void switchSysLanguage(QLocale const& kNewLocale)
{
    QString prefix = "qt";
    static QMap<QString, QTranslator*> Translators;
    kNewLocale.bcp47Name();
    QTranslator *translator = Translators.value(prefix);
    if(translator == NULL){
        translator = new QTranslator(QCoreApplication::instance());
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QString path  = QLibraryInfo::path(QLibraryInfo::TranslationsPath);
#else        
        QString path  = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
#endif
        if (translator->load(kNewLocale, prefix, "_", path, ".qm")) {
            Translators[prefix] = translator;
        } else {
            qWarning("Could not load translation file %s in directory %s.",
                     qPrintable(prefix+"_" +kNewLocale.bcp47Name()), qPrintable(path));

        }
    }

    QCoreApplication::installTranslator(translator);
}

void LanguageMgr::switchLanguage(const QString& locale, const QString & prefix, const QString & path)
{
    qInfo() << "switchLanguage: " << locale;
    QString lo = locale;
    if(locale==LANG_AUTO_TEXT){
        // QLocale l;
        lo = QLocale::system().name();
        qInfo() << "QLocale::system().name(): " << lo;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        qInfo() << "QLocale::system().nativeTerritoryName(): " << QLocale::system().nativeTerritoryName();
#else        
        qInfo() << "QLocale::system().nativeCountryName(): " << QLocale::system().nativeCountryName();
#endif        
        QString lang;
        QString country;
        if(lo.split("_").size()>=2){
            lang = lo.split("_")[0];
            country = lo.split("_")[1];
        }
        qInfo() << "QLocale::system().uiLanguages(): " << QLocale::system().uiLanguages();        
        //判断是否与uiLangeuages的第一种语言相一致，如果不一致，以uiLanguage为准
        //uiLanguages有可能为空
        if(QLocale::system().uiLanguages().size()>0){
            QString uiLang = QLocale::system().uiLanguages()[0].split("-")[0];
            if (uiLang != lang)
            {
                lo = uiLang + "_"+ country;
            }
        }

        qInfo() << lo;
        //获取系统中的所有语言
        QStringList languages = getLanguage(path, prefix).keys();
        //如果系统不存在此翻译    
        if (!languages.contains(lo))
        {
            QString lanKey = lo.split("_")[0];
            foreach (QString key, languages)
            {
                if (key.split("_")[0] == lanKey)
                {
                    lo = key;
                    break;
                }
            }
        } 
    }
    qInfo() << "switchLanguage2: " << lo;


    QLocale::setDefault(QLocale(lo));
    // QLocale::setDefault(QLocale(QLocale::English,QLocale::UnitedStates));

    g_languageCurrentLocale = lo;
    static QMap<QString, QTranslator*> Translators;
    qDebug() << "Translators" << Translators;
    QTranslator *translator = Translators.value(lo);
    if(translator == NULL){
        translator = new QTranslator(QCoreApplication::instance());
        if (translator->load(QLocale(lo), prefix, "_", path, ".qm")) {
            Translators[lo] = translator;     
        } else {
            qWarning("Could not load translation file %s in directory %s.",
                     qPrintable(prefix+"_" +lo), qPrintable(path));
            // 出错后加载英文
            if (translator->isEmpty())
            {
                lo = "en_US";
                g_languageCurrentLocale = lo;
                qWarning("us en_US translation file %s in directory %s.",
                            qPrintable(prefix + "_" + lo), qPrintable(path));                
                if (translator->load(QLocale(lo), prefix, "_", path, ".qm")){
                    Translators[lo] = translator;     
                }else
                {
                    qWarning("Could not load translation file %s in directory %s.",
                             qPrintable(prefix + "_" + lo), qPrintable(path));
                }
            }
            QLocale::setDefault(QLocale(lo));
        }
    }

    QCoreApplication::installTranslator(translator);

    //qt自带的翻译
    switchSysLanguage(QLocale(lo));

}

QString LanguageMgr::getCurrentPrefix()
{
    return LanguageMgr::g_languageCurrentLocale;
}

QString LanguageMgr::getCurrentLanguageName()
{
    QLocale c(g_languageCurrentLocale);
    QString name ="";
    if(QLocale::English == c.language()){
        name += QLocale::languageToString(c.language());
    }
    else{
        name += c.nativeLanguageName();
    }
    return name;
}


/*
void LanguageMgr::retranslateUi(class QObject* parent)
{
    qDebug() << "retranslateUi";
    QList<QLabel *> labels = parent->findChildren<QLabel *>();
    foreach(QLabel* label, labels){
        QByteArray ba = label->property("initText").toByteArray();
        qDebug() << ba;
        qDebug() << label->text();
        if(!ba.isEmpty()){
            QString text = QObject::tr(ba.data());
            label->setText(text);
        }
    }
    QList<QPushButton *> buttons = parent->findChildren<QPushButton *>();
    foreach(QPushButton* button, buttons){
        QByteArray ba = button->property("initText").toByteArray();
        if(!ba.isEmpty()){
            QString text = QObject::tr(ba.data());
            button->setText(text);
        }
qDebug() <<button;
    }

    QList<QAction*> actions = parent->findChildren<QAction*>();
    foreach(QAction* act, actions){
        QByteArray ba = act->property("initText").toByteArray();
        if(!ba.isEmpty()){
            QString text = QObject::tr(ba.data());
            act->setText(text);
        }
        QByteArray st = act->property("StatusTip").toByteArray();
        if(!st.isEmpty()){
            QString text = QObject::tr(st.data());
            act->setStatusTip(text);
        }
    }

    QList<QToolBar *> toolBars = parent->findChildren<QToolBar *>();
    foreach(QToolBar* tb, toolBars){
        QByteArray ba = tb->property("initText").toByteArray();
        if(!ba.isEmpty()){
            QString text = QObject::tr(ba.data());
            tb->setWindowTitle(text);
        }
    }

    QList<QMenu *> menus = parent->findChildren<QMenu *>();
    foreach(QMenu* menu, menus){
        QByteArray ba = menu->property("initText").toByteArray();
        if(!ba.isEmpty()){
            QString text = QObject::tr(ba.data());
            menu->setTitle(text);
        }
    }

    QList<QDockWidget *> docks = parent->findChildren<QDockWidget *>();
    foreach(QDockWidget* dock, docks){
        QByteArray ba = dock->property("initText").toByteArray();
        if(!ba.isEmpty()){
            QString text = QObject::tr(ba.data());
            dock->setWindowTitle(text);
        }

    }

}
*/
