#include "versiondialog.h"

#include <QTextEdit>
#include <QPushButton>
#include <QDesktopServices>

#include "src/common.h"
#include "src/ui/lang/language.h"

VersionDialog::VersionDialog(const VersionConfig& config, QWidget *parent) : RevoDialog(parent), m_config(config)
{
    m_title = m_config.manual ? LAN_VERSION_UPDATE : LAN_SOFTWARE_UPGRADE_TIP;
    setFixedWidth(RV_MESSAGE_BOX_FIX_WIDTH);

    initUi();
}

void VersionDialog::initContent()
{

    QVBoxLayout* textVLayout = new QVBoxLayout();
    textVLayout->setContentsMargins(20, 0, 20, 0);
    textVLayout->setSpacing(0);

    QString versionText = m_config.manual ? QString(LAN_METRO_LASTEST_VERSION_UPDATE_TIP).arg(m_config.version) : LAN_SOFTWARE_UPGRADE_TEXT;

    QLabel* versionLabel = new QLabel(versionText, this);
    versionLabel->setContentsMargins(0,10,0,10);
    versionLabel->setObjectName("labelContent");
    versionLabel->setStyleSheet(versionLabel->styleSheet() + "QLabel{color: #FFFFFF;}");
    versionLabel->setWordWrap(true);

    QTextEdit* versionDescWgt = new QTextEdit(this);
    versionDescWgt->setContentsMargins(0,0,0,0);
    versionDescWgt->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 隐藏垂直滚动条
    versionDescWgt->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 隐藏水平滚动条
    versionDescWgt->setText(m_config.description);
    versionDescWgt->setFixedHeight(120);
    versionDescWgt->setFocusPolicy(Qt::NoFocus);
    versionDescWgt->setReadOnly(true);
    versionDescWgt->setTextInteractionFlags(Qt::NoTextInteraction);
    versionDescWgt->setStyleSheet(R"(
        font-family: Source Han Sans CN Regular;
        color: #FFFFFF;
        font-size: 14px;
        border: null;
        background-color: #2B2B2B;
        padding: 10px 12px 10px 12px;
    )");

    textVLayout->addWidget(versionLabel);
    textVLayout->addWidget(versionDescWgt);

    m_mainLayout->addLayout(textVLayout);
}

void VersionDialog::initButton()
{
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(20, 16, 20, 16);

#ifdef _WIN32
    btnLayout->setSpacing(10);
#elif __APPLE__
    btnLayout->setSpacing(20);
#endif

    btnLayout->addStretch();

    QPushButton* leftBtn = new QPushButton();
    leftBtn->setText(LAN_CANCEL);
    leftBtn->setStyleSheet(leftBtnStyle);

    btnLayout->addWidget(leftBtn);

    connect(leftBtn, &QPushButton::clicked, [=] {
        close();
    });

    QString versionText = m_config.manual ? LAN_CONFIRM : LAN_UPDATE;

    QPushButton* rightBtn = new QPushButton();
    rightBtn->setText(versionText);
    rightBtn->setStyleSheet(rightBtnStyle);
    btnLayout->addWidget(rightBtn);

    connect(rightBtn, &QPushButton::clicked, [=] {
        QDesktopServices::openUrl(QUrl(m_config.url));
        close();

    });
    rightBtn->setFocus();


    m_mainLayout->addLayout(btnLayout);
}

