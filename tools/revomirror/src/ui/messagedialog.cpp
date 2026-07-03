#include "messagedialog.h"

#include <QLabel>
#include <QDebug>
#include <QPushButton>

#include "src/ui/lang/language.h"
#include "src/common.h"

MessageDialog::MessageDialog(const MessageDialogConfig& config, QWidget* parent) : RevoDialog(parent), m_config(config)
{
    m_title = m_config.title;
    setFixedWidth(RV_MESSAGE_BOX_FIX_WIDTH);
    initUi();
}

void MessageDialog::information(const QString &title, const QString &content, QWidget *parent)
{
    MessageDialog dialog(MessageDialogConfig{title, content, "", LAN_CONFIRM}, parent);
    dialog.exec();
}

void MessageDialog::initContent()
{
    QHBoxLayout* contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(20, 20, 20, 0);
    contentLayout->setSpacing(0);

    QLabel* contentLabel = new QLabel(this);
    contentLabel->setMinimumHeight(80);
    contentLabel->setObjectName("labelContent");
    contentLabel->setStyleSheet(R"(
        QLabel {
            font-family: Source Han Sans CN Regular;
            color: #FFFFFF;
            font-size: 14px;
        })");
    contentLabel->setWordWrap(true);
    contentLabel->setText(m_config.content);
    contentLayout->addWidget(contentLabel, 0, Qt::AlignLeft);

    m_mainLayout->addLayout(contentLayout);
}

void MessageDialog::initButton()
{
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(20, 15, 20, 15);

#ifdef _WIN32
    btnLayout->setSpacing(10);
#elif __APPLE__
    btnLayout->setSpacing(20);
#endif

    btnLayout->addStretch();

    if(!m_config.left.isEmpty())
    {
        QPushButton* leftBtn = new QPushButton();
        leftBtn->setText(m_config.left);
        leftBtn->setStyleSheet(leftBtnStyle);

        connect(leftBtn, &QPushButton::clicked, [=] {
            if(m_config.leftCb)
            {
                m_config.leftCb();
                return;
            }
            close();
        });


        btnLayout->addWidget(leftBtn);
    }

    if(!m_config.right.isEmpty())
    {

        QPushButton* rightBtn = new QPushButton();
        rightBtn->setText(m_config.right);
        rightBtn->setStyleSheet(rightBtnStyle);
        btnLayout->addWidget(rightBtn);

        connect(rightBtn, &QPushButton::clicked, [=] {
            if(m_config.rightCb)
            {
                m_config.rightCb();
                return;
            }
            close();

        });
        rightBtn->setFocus();
    }


    m_mainLayout->addLayout(btnLayout);
}
