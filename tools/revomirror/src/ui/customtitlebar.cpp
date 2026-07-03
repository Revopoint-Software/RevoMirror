#include "customtitlebar.h"

#include <QEvent>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QFrame>
#include <QSettings>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QDebug>
#include <QDesktopServices>

#include "src/common.h"
#include "src/ui/lang/languagemgr.h"
#include "src/ui/lang/language.h"

QString findProgramPath(const QString &programName) {
    // 常见的注册表路径
    QStringList registryPaths = {
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall",
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall",
        "HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"
    };

    for (const QString &basePath : registryPaths) {
        QSettings settings(basePath, QSettings::NativeFormat);
        QStringList groups = settings.childGroups();

        for (const QString &group : groups) {
            if(programName == group)
            {
                QString path = settings.value(programName + "/DisplayIcon").toString();
                return path;
            }
        }
    }

    return QString(); // 未找到
}


CustomTitleBar::CustomTitleBar(QWidget *parent)
    : QWidget(parent), m_dragging(false)
{
    resize(TITILE_BAR_W, TITILE_BAR_H);

    // 创建布局
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(0);

    // 标题标签
    m_iconLabel = new QLabel();
    m_iconLabel->setPixmap(QPixmap(":/resources/images/icon_logo_about.png").scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // 标题标签
    m_titleLabel = new QLabel("Revo Mirror");
    m_titleLabel->setObjectName("labelTitle");
    m_titleLabel->setStyleSheet("font-weight: bold; margin-left: 8px");

    // 用户手册按钮
    m_userManualBtn = new QPushButton();
    m_userManualBtn->setObjectName("userManualBtn");
    m_userManualBtn->setStyleSheet("margin-right: 12px");
    m_userManualBtn->setToolTip(LAN_USER_MANUAL);

    // 关于按钮
    m_aboutButton = new QPushButton();
    m_aboutButton->setObjectName("aboutBtn");
    m_aboutButton->setStyleSheet("margin-right: 12px");
    m_aboutButton->setToolTip(LAN_ABOUT);

    QLabel *line = new QLabel();
    line->setStyleSheet("background-color:#555555;");
    line->setFixedSize(1, 12);  // 设置固定尺寸

    m_minimizeButton = new QPushButton();
    m_minimizeButton->setObjectName("minBtn");
    m_minimizeButton->setToolTip(LAN_WIN_MINIMIZE);
    m_maximizeButton = new QPushButton();
    m_maximizeButton->setVisible(false);
    m_maximizeButton->setObjectName("maxBtn");
    m_maximizeButton->setEnabled(false);
    m_maximizeButton->setToolTip(LAN_WIN_MAXIMIZE);
    m_closeButton = new QPushButton();
    m_closeButton->setObjectName("closeBtn");
    m_closeButton->setToolTip(LAN_WIN_CLOSE);

    // 添加到布局
    layout->addWidget(m_iconLabel);
    layout->addWidget(m_titleLabel);
    layout->addStretch();
    layout->addWidget(m_userManualBtn);
    layout->addWidget(m_aboutButton);
    layout->addWidget(line); // 假设你已经有一个布局
    layout->addWidget(m_minimizeButton);
    layout->addWidget(m_maximizeButton);
    layout->addWidget(m_closeButton);

    // 连接信号
    connect(m_minimizeButton, &QPushButton::clicked, this, &CustomTitleBar::onMinimizeClicked);
    connect(m_closeButton, &QPushButton::clicked, this, &CustomTitleBar::onCloseWindow);
    connect(m_aboutButton, &QPushButton::clicked, this, &CustomTitleBar::signalOpenMoreDialog);
    connect(m_userManualBtn, &QPushButton::clicked, this, &CustomTitleBar::onOpenUserManual);
}

void CustomTitleBar::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        m_userManualBtn->setToolTip(LAN_USER_MANUAL);
        m_aboutButton->setToolTip(LAN_ABOUT);
        m_minimizeButton->setToolTip(LAN_WIN_MINIMIZE);
        m_maximizeButton->setToolTip(LAN_WIN_MAXIMIZE);
        m_closeButton->setToolTip(LAN_WIN_CLOSE);
    }
    QWidget::changeEvent(event);
}

void CustomTitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragPosition = event->globalPos() - parentWidget()->frameGeometry().topLeft();
    }
}

void CustomTitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        parentWidget()->move(event->globalPos() - m_dragPosition);
    }
}

void CustomTitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    m_dragging = false;
}

void CustomTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
}

void CustomTitleBar::onMinimizeClicked()
{
    parentWidget()->showMinimized();
}

void CustomTitleBar::onCloseWindow()
{
    parentWidget()->close();
}

void CustomTitleBar::onOpenUserManual()
{
    auto lang = LanguageMgr::getCurrentPrefix();
    QString link = USER_MANUAL_LINK_EN;
    if ("zh_CN" == lang)
    {
        link = USER_MANUAL_LINK_ZH;
    }
    else if ("ja" == lang)
    {
        link = USER_MANUAL_LINK_JA;
    }

    QDesktopServices::openUrl(QUrl(link));
}
