#ifndef MIRRORPAIR_H
#define MIRRORPAIR_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

#include "src/common.h"

class MirrorPair : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool bNewPair READ isNewPair WRITE setNewPair NOTIFY bNewPairChanged)
    Q_PROPERTY(bool bWaiting READ isWaiting WRITE setWaiting NOTIFY bWaitingChanged)
    Q_PROPERTY(QString errorText READ errorText WRITE setErrorText NOTIFY errorTextChanged)
    Q_PROPERTY(int pairState READ pairState WRITE setPairState NOTIFY pairStateChanged)

    enum class ErrorType
    {
        PIN_EMPTY,
        PIN_INVALID
    };
public:
    MirrorPair(QWidget* parent = nullptr);

    void init(const WifiInfo& info);

    void initUi();

    void initNewPairBtn();

    void initWifiUi();

    void initWaitUi();

    void changeEvent(QEvent *event) override;

    bool isNewPair() const;
    void setNewPair(bool newBNewPair);

    bool isWaiting() const;
    void setWaiting(bool newBWaiting);

    const QString &errorText() const;
    void setErrorText(const QString &newErrorText);

    int pairState() const;
    void setPairState(int newPairState);

public slots:
    void slotPairComplete(bool success);
    void slotbWaitingChanged();
private:
    void startPair();

    void setPinErrorVisible(bool visible);

    QString getSupportWifiDeviceText();
signals:
    void bNewPairChanged();

    void bWaitingChanged();

    void signalPairStart(const QString& devName, const QString& pinCode);
    void signalCancelBtnVisible(bool visible);
    void signalNePairBtnVisible(bool visible);

    void errorTextChanged();

    void pairStateChanged();

private:
    QVBoxLayout* m_mainLayout{nullptr};

    QPushButton* m_cancelBtn{nullptr};
    QPushButton* m_newPairBtn{nullptr};
    QWidget* m_placeholderJump{nullptr};
    QLabel* m_pairHintLabel{nullptr};
    QLabel* m_devNameHintLabel{nullptr};


    QLabel* m_wifiHintLabel{nullptr};

    QLabel* m_pswdHintLabel{nullptr};
    QLabel* m_nameHintLabel{nullptr};


    QLabel* m_pinLabel{nullptr};
    QLineEdit* m_pinCodeEdit{nullptr};
    QPushButton* m_errorBtn{nullptr};

    QWidget* m_placeholderError{nullptr};

    QPushButton* m_mirrorBtn{nullptr};

    bool m_bNewPair{false};
    bool m_bWaiting{false};

    WifiInfo m_wifiInfo;

    QString m_devName;
    QString m_pinCode;
    QString m_errorText;
    int m_pairState{static_cast<int>(PairState::INVALID)};

    ErrorType m_pinErrorType{ErrorType::PIN_EMPTY};
    QMap<int, QString> m_mapError;
};

#endif // MIRRORPAIR_H
