// gradientline.cpp
#include "gradientline.h"
#include <QPainter>
#include <QLinearGradient>
#include <QPaintEvent>

GradientLine::GradientLine(QWidget *parent)
    : QWidget(parent)
    , m_lineWidth(2)
    , m_lineHeight(140)
    , m_centerColor(Qt::black)
    , m_direction(Vertical)
    , m_gradientSmooth(true)
{
    updateGeometry();
    setAttribute(Qt::WA_TranslucentBackground, true);
}

GradientLine::GradientLine(int width, int height, QColor centerColor,
                          Direction direction, QWidget *parent)
    : QWidget(parent)
    , m_lineWidth(width)
    , m_lineHeight(height)
    , m_centerColor(centerColor)
    , m_direction(direction)
    , m_gradientSmooth(true)
{
    updateGeometry();
    setAttribute(Qt::WA_TranslucentBackground, true);
}

void GradientLine::setLineSize(int width, int height)
{
    if (m_lineWidth != width || m_lineHeight != height) {
        m_lineWidth = width;
        m_lineHeight = height;
        updateGeometry();
        update();
    }
}

void GradientLine::setCenterColor(const QColor &color)
{
    if (m_centerColor != color) {
        m_centerColor = color;
        update();
    }
}

void GradientLine::setDirection(Direction direction)
{
    if (m_direction != direction) {
        m_direction = direction;
        updateGeometry();
        update();
    }
}

void GradientLine::setGradientSmooth(bool smooth)
{
    if (m_gradientSmooth != smooth) {
        m_gradientSmooth = smooth;
        update();
    }
}

void GradientLine::updateGeometry()
{
    if (m_direction == Vertical) {
        setFixedSize(m_lineWidth, m_lineHeight);
    } else {
        setFixedSize(m_lineHeight, m_lineWidth);
    }
}

void GradientLine::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    if (m_gradientSmooth) {
        painter.setRenderHint(QPainter::Antialiasing, true);
    }

    QRect drawRect = rect();
    QLinearGradient gradient;

    if (m_direction == Vertical) {
        // 垂直渐变：从上到下
        gradient = QLinearGradient(0, 0, 0, height());
    } else {
        // 水平渐变：从左到右
        gradient = QLinearGradient(0, 0, width(), 0);
    }

    // 设置渐变颜色：两端透明，中间为指定颜色
    QColor transparentColor = m_centerColor;
    transparentColor.setAlpha(0);

    gradient.setColorAt(0.0, transparentColor);
    gradient.setColorAt(0.5, m_centerColor);
    gradient.setColorAt(1.0, transparentColor);

    // 填充整个widget区域
    painter.fillRect(drawRect, QBrush(gradient));
}
