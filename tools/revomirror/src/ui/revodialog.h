#ifndef REVODIALOG_H
#define REVODIALOG_H

#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>

const QString leftBtnStyle = QString(R"(
    /* 按钮样式 */
    QPushButton {
        font-family: Source Han Sans CN Regular;
        font-size: 14px;
        min-width: 56px;
        max-width: 120px;
        color: #FFFFFF;
        border: 1px solid #8C8C8C;
        border-radius: 4px;
        padding: 8px 12px 8px 12px ;
    }

    QPushButton:hover {
        border-color: #FFFFFF;
    }

)");

const QString rightBtnStyle = QString(R"(
    /* 按钮样式 */
    QPushButton {
        font-family: Source Han Sans CN Regular;
        font-size: 14px;
        min-width: 56px;
        max-width: 120px;
        color: #FFFFFF;
        border: none;
        border-radius: 4px;
        padding: 8px 12px 8px 12px ;
        background-color: #E64545;
    }

    QPushButton:hover {
        background-color: #F2746F;
    }

    QPushButton:pressed {
        background-color: #BF3035;
    }

)");

class RevoDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RevoDialog(QWidget *parent = nullptr);

    void initUi();
private:
    void initContainer();
    void initTitlebar();
    void initSplitLine();
    virtual void initContent();
    virtual void initButton();
signals:

protected:
    QVBoxLayout* m_mainLayout{nullptr};
    QString m_title;
};

#endif // REVODIALOG_H
