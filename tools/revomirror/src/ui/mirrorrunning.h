#ifndef MIRRORRUNNING_H
#define MIRRORRUNNING_H

#include <QWidget>
#include <QLabel>

class MirrorRunning : public QWidget
{
    Q_OBJECT
public:
    MirrorRunning(QWidget* parent = nullptr);

    void initUi();

    void changeEvent(QEvent *event) override;
signals:
    void signalMirrorStopped();
private:
    QLabel* m_stopLabel{nullptr};
};

#endif // MIRRORRUNNING_H
