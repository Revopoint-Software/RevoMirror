#include "dialogcustomtitlebar.h"
#include <QApplication>
#include <QEvent>

#include "src/ui/lang/language.h"

DialogCustomTitleBar::DialogCustomTitleBar(const QString& title, QWidget *parent)
    : QWidget(parent)
    , m_layout(nullptr)
    , m_titleLabel(nullptr)
    , m_closeButton(nullptr)
    , m_dragging(false)
{
    setupUI(title);
}

void DialogCustomTitleBar::setTitle(const QString &title)
{
    m_titleLabel->setText(title);
}

void DialogCustomTitleBar::setDialog(QDialog *dialog)
{
    m_dialog = dialog;
}

void DialogCustomTitleBar::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        m_closeButton->setToolTip(LAN_WIN_CLOSE);
    }
    QWidget::changeEvent(event);
}

void DialogCustomTitleBar::setupUI(const QString& title)
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(20, 12, 20, 12);
    m_layout->setSpacing(0);

    // 标题标签
    m_titleLabel = new QLabel(title, this);
    m_titleLabel->setObjectName("labelTitle");

    // 关闭按钮
    m_closeButton = new QPushButton(this);
    m_closeButton->setObjectName("closeBtn");
    m_closeButton->setStyleSheet("QToolTip{background-color: white; color: black;}");
    m_closeButton->setToolTip(LAN_WIN_CLOSE);

    // 布局
    m_layout->addWidget(m_titleLabel);
    m_layout->addStretch();
    m_layout->addWidget(m_closeButton);

    // 连接信号
    connect(m_closeButton, &QPushButton::clicked, this, &DialogCustomTitleBar::onCloseClicked);
}

void DialogCustomTitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragPosition = event->globalPos() - (m_dialog ? m_dialog :parentWidget())->frameGeometry().topLeft();
        event->accept();
    }
}

void DialogCustomTitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_dragging) {
        (m_dialog ? m_dialog :parentWidget())->move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void DialogCustomTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
}

void DialogCustomTitleBar::onCloseClicked()
{
    emit closeClicked();
}
