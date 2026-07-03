#ifndef LOADINGDIALOG_H
#define LOADINGDIALOG_H

#include <QObject>
#include <QDialog>
#include <QLabel>

class LoadingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoadingDialog(QWidget *parent = nullptr);

public:
    void setLoadingFail();
private:
    void setupUI();

    void changeEvent(QEvent *event) override;
private:
    QLabel* m_loading{nullptr};
};

#endif // LOADINGDIALOG_H
