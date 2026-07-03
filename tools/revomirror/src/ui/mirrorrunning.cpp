#include "mirrorrunning.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QEvent>

#include "src/common.h"
#include "src/ui/lang/language.h"

MirrorRunning::MirrorRunning(QWidget* parent)
{
    initUi();
}

void MirrorRunning::initUi()
{
    resize(CLICENT_W, CLICENT_H);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    QPushButton *stopBtn = new QPushButton();
    stopBtn->setObjectName("stopBtn");

    connect(stopBtn, &QPushButton::clicked, this, &MirrorRunning::signalMirrorStopped);

    mainLayout->addWidget(stopBtn, 0, Qt::AlignCenter);

    m_stopLabel = new QLabel();
    m_stopLabel->setText(LAN_TERMINATE_MIRROR);
    m_stopLabel->setObjectName("labelTitle");
    m_stopLabel->setStyleSheet(m_stopLabel->styleSheet() + R"(QLabel {margin-top: 20px;})");

    mainLayout->addWidget(m_stopLabel, 0, Qt::AlignCenter);
}

void MirrorRunning::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        m_stopLabel->setText(LAN_TERMINATE_MIRROR);
    }
    QWidget::changeEvent(event);
}
