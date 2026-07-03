#ifndef MIRRORSTOPPED_H
#define MIRRORSTOPPED_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class MirrorStopped : public QWidget
{
    Q_OBJECT
public:
    MirrorStopped(QWidget* parent = nullptr);

    void initUi();

    void changeEvent(QEvent *event) override;
signals:
    void signalMirrorRestart();

private:
    QLabel* m_stopLabel{nullptr};
    QPushButton* m_mirrorBtn{nullptr};
};

#endif // MIRRORSTOPPED_H
