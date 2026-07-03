// gradientline.h
#ifndef GRADIENTLINE_H
#define GRADIENTLINE_H

#include <QWidget>
#include <QColor>

class GradientLine : public QWidget
{
    Q_OBJECT

public:
    enum Direction {
        Vertical,
        Horizontal
    };

    explicit GradientLine(QWidget *parent = nullptr);
    GradientLine(int width, int height, QColor centerColor = Qt::black,
                Direction direction = Vertical, QWidget *parent = nullptr);

    // 设置线条属性
    void setLineSize(int width, int height);
    void setCenterColor(const QColor &color);
    void setDirection(Direction direction);
    void setGradientSmooth(bool smooth);

    // 获取线条属性
    QColor centerColor() const { return m_centerColor; }
    Direction direction() const { return m_direction; }
    QSize lineSize() const { return QSize(m_lineWidth, m_lineHeight); }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void updateGeometry();

private:
    int m_lineWidth;
    int m_lineHeight;
    QColor m_centerColor;
    Direction m_direction;
    bool m_gradientSmooth;
};

#endif // GRADIENTLINE_H
