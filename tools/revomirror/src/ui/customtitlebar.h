#ifndef CUSTOMTITLEBAR_H
#define CUSTOMTITLEBAR_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTimer>

class CustomTitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit CustomTitleBar(QWidget *parent = nullptr);

    void changeEvent(QEvent *event) override;
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
private slots:
    void onMinimizeClicked();
    void onCloseWindow();
    void onOpenUserManual();
signals:
    void signalOpenMoreDialog();
private:
    QLabel *m_iconLabel{nullptr};
    QLabel *m_titleLabel{nullptr};
    QPushButton *m_userManualBtn{nullptr};
    QPushButton *m_aboutButton{nullptr};
    QPushButton *m_minimizeButton{nullptr};
    QPushButton *m_maximizeButton{nullptr};
    QPushButton *m_closeButton{nullptr};
    QPoint m_dragPosition;
    bool m_dragging;
};

#endif // CUSTOMTITLEBAR_H
