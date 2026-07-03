#include "mirrorstopped.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QEvent>

#include "src/common.h"
#include "src/ui/lang/language.h"

MirrorStopped::MirrorStopped(QWidget* parent)
{
    initUi();
}

void MirrorStopped::initUi()
{
    resize(CLICENT_W, CLICENT_H);


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    // 图片
    QPushButton *iconBtn = new QPushButton();
    iconBtn->setObjectName("disconnectBtn");

    mainLayout->addWidget(iconBtn, 0, Qt::AlignCenter);


    m_stopLabel = new QLabel();
    m_stopLabel->setText(LAN_MIRROR_STOPPED);
    m_stopLabel->setObjectName("labelTitle");
    m_stopLabel->setStyleSheet(m_stopLabel->styleSheet() + R"(margin-top: 16px; height: 24px;)");

    mainLayout->addWidget(m_stopLabel, 0, Qt::AlignCenter);

    m_mirrorBtn = new QPushButton();
    m_mirrorBtn->setText(LAN_RESTART_MIRROR);
    QString additionalStyle = R"(height: 36px;margin-top: 69px;padding: 8px 32px;)";

    m_mirrorBtn->setStyleSheet(m_mirrorBtn->styleSheet() + additionalStyle);

    mainLayout->addWidget(m_mirrorBtn);

    connect(m_mirrorBtn, &QPushButton::clicked, this, &MirrorStopped::signalMirrorRestart);
}

void MirrorStopped::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        m_stopLabel->setText(LAN_MIRROR_STOPPED);
        m_mirrorBtn->setText(LAN_RESTART_MIRROR);
    }
    QWidget::changeEvent(event);
}
