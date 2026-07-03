#ifndef MOREDIALOG_H
#define MOREDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QStyledItemDelegate>
#include "dialogcustomtitlebar.h"

class AboutPage;
class SettingsPage;
class NoShadowComboBox;

class MoreDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MoreDialog(QWidget *parent = nullptr);
    ~MoreDialog();

    void changeEvent(QEvent *event) override;

    static void setSelectLanguage(const QString& language);
signals:
    void signalSelectLanguage(int lan);
private slots:
    void onMenuItemClicked(int index);
    void onTitleBarCloseClicked();

private:
    void setupUI();
    void setupTitleBar();
    void setupContent();
    void setupMenuList();
    void setupPages();

private:
    QVBoxLayout* m_mainLayout;
    DialogCustomTitleBar* m_titleBar;
    QWidget* m_contentWidget;
    QListWidget* m_menuList;
    QStackedWidget* m_pageStack;

    QListWidgetItem* m_aboutMenuItem{nullptr};
    QListWidgetItem* m_settingMenuItem{nullptr};

    AboutPage* m_aboutPage;
    SettingsPage* m_settingsPage;

    static QString g_selectLanguage;
};

class ImageTextItemDelegate : public QStyledItemDelegate
{
public:
    ImageTextItemDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

// 关于页面
class AboutPage : public QWidget
{
    Q_OBJECT
public:
    explicit AboutPage(QWidget* parent = nullptr);

    void changeEvent(QEvent *event) override;

    void reset();
private:
    QString getUserAgreementText();
    QString getPrivacyPolicyText();
    QString getThridCopyrightText();

    void checkAndUpdate();
private:
    MoreDialog* m_dialog{nullptr};

    QPushButton* m_updateBtn{nullptr};
    QLabel* m_updateMoviceLabel{nullptr};

    QLabel* m_thridCopyright{nullptr};
    QLabel* m_privacyPolicy{nullptr};
    QLabel* m_userAgreement{nullptr};
};

// 设置页面
class SettingsPage : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsPage(QWidget* parent = nullptr);
    void changeEvent(QEvent *event) override;
private:
    MoreDialog* m_dialog{nullptr};
    NoShadowComboBox* m_langComboBox{nullptr};

    QLabel* m_languageLabel{nullptr};
};


#endif // MOREDIALOG_H
