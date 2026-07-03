#include "revodialog.h"

#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QDialog>
#include <QWidget>
#include <QScreen>
#include <QApplication>
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
#include <QDesktopWidget>
#endif
#include <QRect>
#include <QPoint>
#include <QTimer>

#include "dialogcustomtitlebar.h"

class DialogUtils
{
public:
    // 将对话框移动到父窗口中心，处理屏幕边界
    static void centerDialogOnParent(QDialog *dialog, QWidget *parent = nullptr)
    {
        if (!dialog) return;

        // 如果没有指定父窗口，使用应用程序的活动窗口
        if (!parent) {
            parent = QApplication::activeWindow();
        }

        // 如果仍然没有父窗口，居中到屏幕
        if (!parent) {
            centerDialogOnScreen(dialog);
            return;
        }

        // 获取父窗口的几何信息
        QRect parentGeometry = parent->geometry();
        QSize dialogSize = dialog->size();

//        qInfo() << "parentGeometry" << parentGeometry << "bottomRight" << parentGeometry.bottomRight();
//        qInfo() << "dialogSize" << dialogSize;

        // 计算居中位置
        int x = parentGeometry.x() + (parentGeometry.width() - dialogSize.width()) / 2;
        int y = parentGeometry.y() + (parentGeometry.height() - dialogSize.height()) / 2;

        // 处理屏幕边界
        QPoint finalPosition = adjustPositionForScreen(QPoint(x, y), dialogSize, parent);

//        qInfo() << "finalPosition" << finalPosition;

        dialog->move(finalPosition);
    }

    // 居中到屏幕
    static void centerDialogOnScreen(QDialog *dialog, QWidget *referenceWidget = nullptr)
    {
        if (!dialog) return;

        QRect screenGeometry = getAvailableGeometry(referenceWidget);
        QSize dialogSize = dialog->size();

        int x = screenGeometry.x() + (screenGeometry.width() - dialogSize.width()) / 2;
        int y = screenGeometry.y() + (screenGeometry.height() - dialogSize.height()) / 2;

        QPoint finalPosition = adjustPositionForScreen(QPoint(x, y), dialogSize, referenceWidget);
        dialog->move(finalPosition);
    }

private:
    // 获取可用的屏幕几何信息（兼容版本）
    static QRect getAvailableGeometry(QWidget *widget)
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        // Qt 5.14+ 版本
        QScreen *screen = widget ? widget->screen() : QApplication::primaryScreen();
        if (screen) {
            return screen->availableGeometry();
        }
#else
        // 兼容旧版本的方法
        QDesktopWidget *desktop = QApplication::desktop();
        if (desktop) {
            int screenIndex = widget ? desktop->screenNumber(widget) : desktop->primaryScreen();
            return desktop->availableGeometry(screenIndex);
        }
#endif
        // 最后的备用方案
        return QRect(0, 0, 1024, 768);
    }

    // 调整位置以确保在屏幕内，边距：水平20，垂直80
    static QPoint adjustPositionForScreen(const QPoint &position, const QSize &dialogSize, QWidget *referenceWidget)
    {
        const int horizontalMargin = 20;
        const int verticalMargin = 80;

        QRect screenGeometry = getAvailableGeometry(referenceWidget);

        int x = position.x();
        int y = position.y();

        // 检查左边界
        if (x < screenGeometry.x() + horizontalMargin) {
            x = screenGeometry.x() + horizontalMargin;
        }

        // 检查右边界
        if (x + dialogSize.width() > screenGeometry.right() - horizontalMargin) {
            x = screenGeometry.right() - dialogSize.width() - horizontalMargin;
        }

        // 检查上边界
        if (y < screenGeometry.y() + verticalMargin) {
            y = screenGeometry.y() + verticalMargin;
        }

        // 检查下边界
        if (y + dialogSize.height() > screenGeometry.bottom() - verticalMargin) {
            y = screenGeometry.bottom() - dialogSize.height() - verticalMargin;
        }

        return QPoint(x, y);
    }
};

RevoDialog::RevoDialog(QWidget *parent) : QDialog(parent ? parent : QApplication::activeWindow())
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setContentsMargins(0, 0, 0, 0);
}

void RevoDialog::initUi()
{
    initContainer();
    initTitlebar();
    initSplitLine();
    initContent();
    initButton();

    QTimer::singleShot(10, [this]() {
        DialogUtils::centerDialogOnParent(this, parentWidget());
    });
}

void RevoDialog::initContainer()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10,10,10,10);
    mainLayout->setSpacing(0);

    QWidget* containerWidget = new QWidget(this);
    containerWidget->setContentsMargins(0,0,0,0);
    containerWidget->setStyleSheet(
        "background-color: #383838;"
        "border-radius: 8px;");

    m_mainLayout = new QVBoxLayout(containerWidget);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // 创建阴影效果
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);

    // 设置阴影参数
    shadowEffect->setBlurRadius(10);        // 模糊半径
    shadowEffect->setColor(QColor(0, 0, 0, 100));  // 阴影颜色和透明度
    shadowEffect->setOffset(0, 0);          // 阴影偏移

    // 应用阴影效果到中心部件
    containerWidget->setGraphicsEffect(shadowEffect);

    mainLayout->addWidget(containerWidget);

    setLayout(mainLayout);
}

void RevoDialog::initTitlebar()
{
    DialogCustomTitleBar* titleBar = new DialogCustomTitleBar(m_title, this);
    titleBar->setDialog(this);
    connect(titleBar, &DialogCustomTitleBar::closeClicked, this, &RevoDialog::close);

    m_mainLayout->addWidget(titleBar);
}

void RevoDialog::initSplitLine()
{
    QLabel* line = new QLabel(this);
    line->setStyleSheet("background-color:#595959;");
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    line->setFixedHeight(1);  // 设置固定尺寸

    m_mainLayout->addWidget(line);
}

void RevoDialog::initContent()
{

}

void RevoDialog::initButton()
{

}
