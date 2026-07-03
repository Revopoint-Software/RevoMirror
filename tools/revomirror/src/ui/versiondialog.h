#ifndef VERSIONDIALOG_H
#define VERSIONDIALOG_H

#include "revodialog.h"

struct VersionConfig
{
    bool manual{true};
    QString version;
    QString description;
    QString url;
};

class VersionDialog : public RevoDialog
{
    Q_OBJECT
public:
    explicit VersionDialog(const VersionConfig& config, QWidget *parent = nullptr);
private:
    void initContainer();
    void initContent() override;
    void initButton() override;
signals:

private:
    VersionConfig m_config;

    QWidget* m_containerWidget{nullptr};
};

#endif // VERSIONDIALOG_H
