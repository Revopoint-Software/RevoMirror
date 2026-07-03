#ifndef DIALOGCUSTOMTITLEBAR_H
#define DIALOGCUSTOMTITLEBAR_H

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMouseEvent>

class DialogCustomTitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit DialogCustomTitleBar(const QString& title, QWidget *parent = nullptr);

    void setTitle(const QString& title);

    void setDialog(QDialog* dialog);

    void changeEvent(QEvent *event) override;
signals:
    void closeClicked();
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
private slots:
    void onCloseClicked();
private:
    void setupUI(const QString& title);
private:
    QHBoxLayout* m_layout{nullptr};
    QLabel* m_titleLabel{nullptr};
    QPushButton* m_closeButton{nullptr};

    QPoint m_dragPosition;
    bool m_dragging;

    QDialog* m_dialog{nullptr};
};

#endif // DIALOGCUSTOMTITLEBAR_H
