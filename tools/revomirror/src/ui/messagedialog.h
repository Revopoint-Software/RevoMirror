#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include "revodialog.h"

#include <functional>


struct MessageDialogConfig
{
    QString title;
    QString content;
    QString left;
    QString right;
    std::function<void()> leftCb{nullptr};
    std::function<void()> rightCb{nullptr};
};

class MessageDialog : public RevoDialog
{
    Q_OBJECT
public:
    explicit MessageDialog(const MessageDialogConfig& config, QWidget *parent = nullptr);

    static void information(const QString& title, const QString& content, QWidget *parent = nullptr);
private:
    void initContent();
    void initButton();
signals:

private:
    MessageDialogConfig m_config;
};

#endif // MESSAGEDIALOG_H
