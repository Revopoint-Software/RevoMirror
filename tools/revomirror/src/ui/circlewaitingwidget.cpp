#include "circlewaitingwidget.h"

#include <QPainter>
#include <QTimer>
#include <QConicalGradient>

struct CircleWaitingWidget::CircleWidgetImp
{
    int m_angle{ 0 };
    //int m_span{ 120 };
    QTimer* m_timer{ nullptr };
    /// 环状颜色
    QString m_color{ "#000000" };

    /// 文字
    QString m_centerText{""};

    /// 文字颜色
    QString m_centerTextColor{"#222222"};

    /// 文字大小
    int m_centerTextPointSize{ 20 };

    /// 字体加粗
    bool m_centerTextBold{ false };

    /// 旋转方向, true-顺时针，false-逆时针
    bool m_rotationIsClockWise{ true };
};

CircleWaitingWidget::CircleWaitingWidget(QWidget* parent /* = nullptr */) : QWidget(parent)
{
    m_imp = new(std::nothrow) CircleWidgetImp;

    m_imp->m_timer = new QTimer(this);
    connect(m_imp->m_timer, &QTimer::timeout, [this]()
            {
                if (true == m_imp->m_rotationIsClockWise)
                {
                    m_imp->m_angle = (m_imp->m_angle + 2) % 360;
                }
                else
                {
                    m_imp->m_angle = (m_imp->m_angle - 2) % 360;
                }

                update();
            });
    m_imp->m_timer->start(15);
}

CircleWaitingWidget::~CircleWaitingWidget()
{
    m_imp->m_timer->stop();
    delete m_imp->m_timer;
    delete m_imp;
}

///
/// @brief:	设置换装等待的颜色
/// @param:	color
/// @return:	void
///
void CircleWaitingWidget::setFillColor(const QString& color)
{
    m_imp->m_color = color;
    update();
}

///
/// @brief:	setCenterText
/// @param:	str
/// @return:	void
///
void CircleWaitingWidget::setCenterText(const QString& str)
{
    m_imp->m_centerText = str;
    update();
}

///
/// @brief:	setCenterTextColor
/// @param:	color
/// @return:	void
///
void CircleWaitingWidget::setCenterTextColor(const QString& color)
{
    m_imp->m_centerTextColor = color;
    update();
}

///
/// @brief:	setCenterTextPointSize
/// @param:	size
/// @return:	void
///
void CircleWaitingWidget::setCenterTextPointSize(int size)
{
    m_imp->m_centerTextPointSize = size;
    update();
}

///
/// @brief:	设置中间字体加粗
/// @param:	isBold
/// @return:	void
///
void CircleWaitingWidget::setCenterTextBold(bool isBold)
{
    m_imp->m_centerTextBold = isBold;
    update();
}

///
/// @brief:	setRotation
/// @param:	clockWise
/// @return:	void
///
void CircleWaitingWidget::setRotation(bool clockWise)
{
    m_imp->m_rotationIsClockWise = clockWise;
    update();
}

///
/// @brief:	paintEvent
/// @param:	*
/// @return:	void
///
void CircleWaitingWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    const int side = qMin(width(), height());
    painter.translate(width() / 2, height() / 2);
    painter.scale(side / 200.0, side / 200.0);

    QConicalGradient gradient(0, 0, -m_imp->m_angle);
    /// 如果是逆时针
    if (false == m_imp->m_rotationIsClockWise)
    {
        gradient.setColorAt(0.0, QColor(m_imp->m_color));
        gradient.setColorAt(1.0, QColor("#FFFFFF"));
    }
    /// 顺时针
    else
    {
        gradient.setColorAt(1.0, QColor(m_imp->m_color));
        gradient.setColorAt(0.0, QColor("#FFFFFF"));
    }

    QPen pen{};
    pen.setWidth(20);
    pen.setCapStyle(Qt::RoundCap);
    pen.setBrush(gradient);
    painter.setPen(pen);

    int startAngle = 0 * 16;
    int spanAngle = 360 * 16;
    painter.drawArc(-90, -90, 180, 180, startAngle, spanAngle);

    /// 绘制文字
    if (false == m_imp->m_centerText.isEmpty())
    {
        painter.setBrush(Qt::NoBrush);
        QFont tmpFont = painter.font();
        tmpFont.setPointSize(m_imp->m_centerTextPointSize);
        tmpFont.setBold(m_imp->m_centerTextBold);
        painter.setFont(tmpFont);
        painter.setPen(QColor(m_imp->m_centerTextColor));

        painter.drawText(-90, -90, 180, 180, Qt::AlignCenter, m_imp->m_centerText);
    }

}
