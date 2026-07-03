#include "loadingdialog.h"

#include <QEvent>
#include <QVBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QApplication>
#include <QScreen>

#include "src/ui/lang/language.h"
#include "src/ui/circlewaitingwidget.h"
#include "src/common.h"

LoadingDialog::LoadingDialog(QWidget *parent) : QDialog(parent)
{
    setupUI();

  // 居中显示
  QScreen* screen = QApplication::primaryScreen();
  QRect screenGeometry = screen->geometry();
  int x = (screenGeometry.width() - width()) / 2;
  int y = (screenGeometry.height() - height()) / 2;
  move(x, y);

}

void LoadingDialog::setLoadingFail()
{
    //m_loading->setText(LAN_LOADED_FAIL);
}

void LoadingDialog::setupUI()
{
    resize(273,258);    // 设置对话框属性 - 移除系统标题栏
    // 关键：必须设置无边框窗口
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    // 设置透明背景属性
    setAttribute(Qt::WA_TranslucentBackground);

    QWidget* widget = new QWidget(this);
    widget->setContentsMargins(0,0,0,0);
    widget->setGeometry(0, 0, 273, 258);
    widget->setStyleSheet(
        "background-color: transparent;"
        "border-radius: 4px;"
        "background-image: url(:/resources/images/loading_background.png);");

    // 应用图标和名称
    QLabel* appIcon = new QLabel(widget);
    appIcon->setGeometry(80,32,110, 110);

    appIcon->setStyleSheet(
        "QLabel {"
        "    background-image: url(:/resources/images/icon_logo_loading.png);"
        "}"
    );

    QLabel* appName = new QLabel("Revo Mirror", this);
    appName->setStyleSheet(
        "QLabel {"
        "    font-family: Source Han Sans CN Regular;"
        "    margin-top: 156px;"
        "    margin-left: 69px;"
        "    font-size: 24px;"
        "    color: white;"
        "    font-weight: medium;"
        "    border: none;"
        "    background-color: transparent;"
        "}"
    );

    CircleWaitingWidget* circle = new CircleWaitingWidget(this);
    circle->setGeometry((width()-LOADING_CIRCLE_SIZE)/2, 204, LOADING_CIRCLE_SIZE, LOADING_CIRCLE_SIZE);
}

void LoadingDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        //m_loading->setText(LAN_LOADING);
    }
    QDialog::changeEvent(event);
}
