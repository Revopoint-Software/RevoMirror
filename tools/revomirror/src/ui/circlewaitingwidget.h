#pragma once
#include <QWidget>

///
/// 环状等待控件。
///
class CircleWaitingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CircleWaitingWidget(QWidget* parent = nullptr);

    virtual ~CircleWaitingWidget();

    ///
    /// @brief:	设置换装等待的颜色
    /// @param:	color 颜色字符串， 比如"#FF00FF33"
    /// @return:	void
    ///
    void setFillColor(const QString& color);

    ///
    /// @brief:	设置文字
    /// @param:	str
    /// @return:	void
    ///
    void setCenterText(const QString& str);

    ///
    /// @brief:	设置文字颜色
    /// @param:	color
    /// @return:	void
    ///
    void setCenterTextColor(const QString& color);

    ///
    /// @brief:	设置中间文字的大小
    /// @param:	size
    /// @return:	void
    ///
    void setCenterTextPointSize(int size);

    ///
    /// @brief:	设置中间字体加粗
    /// @param:	isBold
    /// @return:	void
    ///
    void setCenterTextBold(bool isBold);


    ///
    /// @brief:	设置旋转方向， true-顺时针， false-逆时针
    /// @param:	clockWise
    /// @return:	void
    ///
    void setRotation(bool clockWise);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    struct CircleWidgetImp;
    CircleWidgetImp* m_imp{ nullptr };
};
