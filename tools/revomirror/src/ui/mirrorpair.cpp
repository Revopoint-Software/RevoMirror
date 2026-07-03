#include "mirrorpair.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QEvent>
#include <QFontMetrics>
#include <QIntValidator>
#include <QShortcut>

#include "src/ui/lang/language.h"
#include "src/ui/lang/languagemgr.h"

MirrorPair::MirrorPair(QWidget* parent)
{
    m_mapError = {
        {static_cast<int>(ErrorType::PIN_EMPTY), LAN_PIN_EMPTY_WARNING},
        {static_cast<int>(ErrorType::PIN_INVALID), LAN_PIN_INVALID_WARNING},
    };
}

void MirrorPair::init(const WifiInfo &info)
{
    m_wifiInfo = info;

    initUi();
}

void MirrorPair::initUi()
{
    resize(CLICENT_W, CLICENT_H);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setAlignment(Qt::AlignTop);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0,0,0,0);

    initNewPairBtn();

    initWifiUi();

    initWaitUi();

    connect(this, &MirrorPair::bWaitingChanged, this, &MirrorPair::slotbWaitingChanged);
}

void MirrorPair::initNewPairBtn()
{

    m_newPairBtn = new QPushButton(LAN_NEW_PAIR, this);
    m_newPairBtn->setIcon(QIcon(":/resources/images/new_pair.png"));
    m_newPairBtn->setObjectName("iconTextBtn");

    // 设置按钮样式，让其内容自适应
    m_newPairBtn->setStyleSheet(R"(
        QPushButton {
            margin-right: 20px;
            padding: 4px 6px 4px 6px;
            text-align: left;
            qproperty-iconSize: 16px 16px;
            color: #E64545;
            font-size: 16px;
            font-family: Source Han Sans CN Regular;

        }
    )");
    // 让按钮根据内容自动调整大小
    m_newPairBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    m_newPairBtn->setVisible(isWaiting());  // 等待连接模式时，显示新配对按钮
    connect(this, &MirrorPair::signalNePairBtnVisible, m_newPairBtn, &QPushButton::setVisible);
    connect(m_newPairBtn, &QPushButton::clicked, [this](){
        setPairState(static_cast<int>(PairState::NEW));
    });

    // 创建占位符（与按钮相同大小）
    m_placeholderJump = new QWidget(this);
    m_placeholderJump->setFixedSize(m_newPairBtn->sizeHint());
    m_placeholderJump->hide(); // 初始隐藏占位符

    m_cancelBtn = new QPushButton(LAN_CANCEL);
    m_cancelBtn->setContentsMargins(0,0,0,0);
    m_cancelBtn->setIcon(QIcon(":/resources/images/cancel.png"));
    m_cancelBtn->setObjectName("iconTextBtn");

    // 设置按钮样式，让其内容自适应
    m_cancelBtn->setStyleSheet(R"(
        QPushButton {
            margin-left: 8px;
            padding: 4px 6px 4px 6px;
            text-align: right;
            qproperty-iconSize: 16px 16px;
            color: #E64545;
            font-size: 16px;
            font-family: Source Han Sans CN Regular;

        }
    )");

    m_cancelBtn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    m_mainLayout->addWidget(m_newPairBtn, 0, Qt::AlignRight);
    m_mainLayout->addWidget(m_placeholderJump, 0, Qt::AlignHCenter);
    m_mainLayout->addWidget(m_cancelBtn, 0, Qt::AlignLeft);

    m_cancelBtn->setVisible(isNewPair());   // 新配对模式时，显示取消按钮
    connect(this, &MirrorPair::signalCancelBtnVisible, m_cancelBtn, &QPushButton::setVisible);
    connect(m_cancelBtn, &QPushButton::clicked, [this](){
        setPairState(static_cast<int>(PairState::PAIRED));
    });

}

void MirrorPair::initWifiUi()
{
    m_wifiHintLabel = new QLabel();
    m_wifiHintLabel->setGeometry(0, 0, 0, 0);
    m_wifiHintLabel->setText(LAN_WIFI_TITLE);
    m_wifiHintLabel->setObjectName("labelTitle");
    m_wifiHintLabel->setWordWrap(true);

    m_wifiHintLabel->setStyleSheet(R"(
        QLabel {
            margin-left: 40px;
            margin-right: 40px;
            margin-top: 10px;
        }
    )");


    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setAlignment(Qt::AlignLeft);

    gridLayout->setContentsMargins(66, 10, 40,10);
    gridLayout->setSpacing(5);

    QString nameText = m_wifiInfo.name.isEmpty() ? LAN_WIFI_NONE_NAME_HINT : LAN_WIFI_NAME;
    m_nameHintLabel = new QLabel(nameText);
    m_nameHintLabel->setWordWrap(true);
    m_nameHintLabel->setObjectName("labelContent");

    QString pswdText = m_wifiInfo.name.isEmpty() ? getSupportWifiDeviceText() : LAN_WIFI_PASSWORD;
    m_pswdHintLabel = new QLabel(pswdText);
    m_pswdHintLabel->setObjectName("labelContent");
    m_pswdHintLabel->setOpenExternalLinks(true);

    gridLayout->addWidget(m_nameHintLabel, 0, 0);
    gridLayout->addWidget(m_pswdHintLabel, 1, 0, Qt::AlignLeft);

    if(!m_wifiInfo.name.isEmpty())
    {
        QLabel* nameLabel = new QLabel(m_wifiInfo.name);
        nameLabel->setObjectName("labelContent");
        QLabel* pswdLabel = new QLabel(m_wifiInfo.password);
        pswdLabel->setObjectName("labelContent");

        gridLayout->addWidget(nameLabel, 0, 1, Qt::AlignLeft);
        gridLayout->addWidget(pswdLabel, 1, 1, Qt::AlignLeft);

        gridLayout->setColumnStretch(0,0);
        gridLayout->setColumnStretch(1,0);
    }
    else
    {
        // 只有一列时，需要拉伸
        gridLayout->setColumnStretch(0,1);
        gridLayout->setColumnStretch(1,0);
    }

    m_mainLayout->addWidget(m_wifiHintLabel);
    m_mainLayout->addLayout(gridLayout);
}

void MirrorPair::initWaitUi()
{
    m_pairHintLabel = new QLabel();
    m_pairHintLabel->setGeometry(0, 0, 0, 0);
    m_pairHintLabel->setText(LAN_PAIR_TITLE);
    m_pairHintLabel->setObjectName("labelTitle");
    m_pairHintLabel->setWordWrap(true);

    m_pairHintLabel->setStyleSheet(R"(
        QLabel {
            margin-left: 40px;
            margin-right: 40px;
            margin-top: 10px;
        }
    )");


    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setAlignment(Qt::AlignLeft);

    gridLayout->setContentsMargins(66, 10, 40,10);
    gridLayout->setSpacing(5);


    m_devNameHintLabel = new QLabel();
    m_devNameHintLabel->setText(LAN_CURRENT_DEVICE_NAME);
    m_devNameHintLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_devNameHintLabel->setObjectName("labelContent");

    QLabel* devNameLabel = new QLabel();
    devNameLabel->setText(QSysInfo::machineHostName());
    devNameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    devNameLabel->setObjectName("labelContent");


    m_pinLabel = new QLabel();
    m_pinLabel->setText(LAN_PIN_NUMBER);
    m_pinLabel->setObjectName("labelContent");

    m_pinCodeEdit = new QLineEdit();
    m_pinCodeEdit->setAlignment(Qt::AlignVCenter);

    // 创建验证器，限制范围0-9999（4位数字）
    QIntValidator *validator = new QIntValidator(0, 9999, m_pinCodeEdit);
    m_pinCodeEdit->setValidator(validator);
    // 设置最大长度为4
    m_pinCodeEdit->setMaxLength(4);

    m_pinCodeEdit->setPlaceholderText(LAN_PIN_PLACEHOLDER);

    // 获取当前调色板
    QPalette palette = m_pinCodeEdit->palette();
    // 设置占位文本颜色
    palette.setColor(QPalette::PlaceholderText, QColor("#737373"));
    // 应用调色板
    m_pinCodeEdit->setPalette(palette);


    connect(m_pinCodeEdit, &QLineEdit::textChanged, [this](const QString& text){
        m_pinCode = text;
        setErrorText("");
    });

    m_errorBtn = new QPushButton(LAN_PIN_EMPTY_WARNING, this);
    m_errorBtn->setIcon(QIcon(":/resources/images/pin_warning.png"));
    m_errorBtn->setEnabled(false);
    m_errorBtn->setVisible(false);

    // 设置按钮样式，让其内容自适应
    m_errorBtn->setStyleSheet(R"(
        QPushButton {
            margin-right: 20px;
            padding: 4px 6px 4px 6px;
            text-align: left;
            qproperty-iconSize: 16px 16px;
            background: none;
            font-family: Source Han Sans CN Regular;
            color: #979797;
            font-size: 14px;
        }
    )");
    // 让按钮根据内容自动调整大小
    m_errorBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    // 创建占位符（与按钮相同大小）
    m_placeholderError = new QWidget(this);
    m_placeholderError->setFixedSize(m_errorBtn->sizeHint());

    m_errorBtn->setText("");

    connect(this, &MirrorPair::errorTextChanged, [this](){
        m_errorBtn->setText(errorText());
        setPinErrorVisible(!errorText().isEmpty());
    });

    gridLayout->addWidget(m_devNameHintLabel, 0, 0, Qt::AlignLeft);
    gridLayout->addWidget(devNameLabel, 0, 1, Qt::AlignLeft);

    gridLayout->addWidget(m_pinLabel, 1, 0, Qt::AlignLeft);
    gridLayout->addWidget(m_pinCodeEdit, 1, 1);

    gridLayout->addWidget(m_errorBtn, 2, 1, Qt::AlignLeft);
    gridLayout->addWidget(m_placeholderError, 2, 1, Qt::AlignLeft);

    gridLayout->setColumnStretch(0,0);
    gridLayout->setColumnStretch(1,1);

    m_mirrorBtn = new QPushButton();
    m_mirrorBtn->setText(LAN_ENTER_MIRROR_MODE);
    m_mirrorBtn->setStyleSheet(R"(
        QPushButton {
            padding: 8px 18px 8px 18px;
            font-family: Source Han Sans CN Regular;
            font-size: 14px;
        }
    )");

    QShortcut* shortcutReturn = new QShortcut(QKeySequence(Qt::Key_Return), m_mirrorBtn);
    QShortcut* shortcutEnter = new QShortcut(QKeySequence(Qt::Key_Enter), m_mirrorBtn);
    connect(shortcutReturn, &QShortcut::activated, m_mirrorBtn, &QPushButton::click);
    connect(shortcutEnter, &QShortcut::activated, m_mirrorBtn, &QPushButton::click);

    connect(m_mirrorBtn, &QPushButton::clicked, this, &MirrorPair::startPair);

    m_mainLayout->addWidget(m_pairHintLabel);

    m_mainLayout->addLayout(gridLayout);
    m_mainLayout->addStretch(1);
    m_mainLayout->addWidget(m_mirrorBtn,0,Qt::AlignCenter);
    m_mainLayout->addSpacing(30);


}

void MirrorPair::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        m_cancelBtn->setText(LAN_CANCEL);
        m_newPairBtn->setText(LAN_NEW_PAIR);
        m_wifiHintLabel->setText(LAN_WIFI_TITLE);
        m_newPairBtn->adjustSize();
        m_pairHintLabel->setText(LAN_PAIR_TITLE);
        m_devNameHintLabel->setText(LAN_CURRENT_DEVICE_NAME);

        if(m_wifiInfo.name.isEmpty())
        {
            m_nameHintLabel->setText(LAN_WIFI_NONE_NAME_HINT);
            m_pswdHintLabel->setText(getSupportWifiDeviceText());
        }
        else
        {
            m_nameHintLabel->setText(LAN_WIFI_NAME);
            m_pswdHintLabel->setText(LAN_WIFI_PASSWORD);
        }

        m_pinLabel->setText(LAN_PIN_NUMBER);
        m_pinCodeEdit->setPlaceholderText(LAN_PIN_PLACEHOLDER);

        switch (m_pinErrorType) {
        case ErrorType::PIN_EMPTY:
            m_errorBtn->setText(LAN_PIN_EMPTY_WARNING);
            break;
        case ErrorType::PIN_INVALID:
            m_errorBtn->setText(LAN_PIN_INVALID_WARNING);
            break;
        default:
            break;
        }


        m_mirrorBtn->setText(LAN_ENTER_MIRROR_MODE);
    }
    QWidget::changeEvent(event);
}

void MirrorPair::startPair()
{
    if(m_pinCode.isEmpty())
    {
        setErrorText(LAN_PIN_EMPTY_WARNING);
        m_pinErrorType = ErrorType::PIN_EMPTY;
        return;
    }
    emit signalPairStart(m_devName, m_pinCode);
}

void MirrorPair::setPinErrorVisible(bool visible)
{
    m_errorBtn->setVisible(visible);
    m_placeholderError->setVisible(!visible);
}

bool MirrorPair::isNewPair() const
{
    return m_bNewPair;
}

void MirrorPair::setNewPair(bool newBNewPair)
{
    if (m_bNewPair == newBNewPair)
        return;
    m_bNewPair = newBNewPair;
    emit bNewPairChanged();
    emit signalCancelBtnVisible(newBNewPair);
}

bool MirrorPair::isWaiting() const
{
    return m_bWaiting;
}

void MirrorPair::setWaiting(bool newBWaiting)
{
    if (m_bWaiting == newBWaiting)
        return;
    m_bWaiting = newBWaiting;
    emit bWaitingChanged();
    emit signalNePairBtnVisible(newBWaiting);
}

void MirrorPair::slotPairComplete(bool success)
{
    if(success)
    {
        return;
    }
    setErrorText(LAN_PIN_INVALID_WARNING);
    m_pinErrorType = ErrorType::PIN_INVALID;
}

void MirrorPair::slotbWaitingChanged()
{
    m_pairHintLabel->setText(isWaiting() ? LAN_PAIR_TITLE : LAN_PAIR_TITLE);
    m_pinLabel->setVisible(!isWaiting());
    m_pinCodeEdit->setVisible(!isWaiting());
    setPinErrorVisible(!isWaiting() && !errorText().isEmpty());
    m_mirrorBtn->setVisible(!isWaiting());
    setErrorText("");
    m_pinCodeEdit->setText("");
}

const QString &MirrorPair::errorText() const
{
    return m_errorText;
}

void MirrorPair::setErrorText(const QString &newErrorText)
{
    if (m_errorText == newErrorText)
        return;
    m_errorText = newErrorText;
    emit errorTextChanged();
}

int MirrorPair::pairState() const
{
    return m_pairState;
}

void MirrorPair::setPairState(int newPairState)
{
    if (m_pairState == newPairState)
        return;
    m_pairState = newPairState;
    emit pairStateChanged();

    switch (newPairState) {
    case static_cast<int>(PairState::NONE):
        setWaiting(false);
        setNewPair(false);
        m_placeholderJump->show();
        break;
    case static_cast<int>(PairState::PAIRED):
        setWaiting(true);
        setNewPair(false);
        m_placeholderJump->hide();
        break;
    case static_cast<int>(PairState::NEW):
        setWaiting(false);
        setNewPair(true);
        m_placeholderJump->hide();
        break;
    default:
        setWaiting(false);
        setNewPair(false);
        m_placeholderJump->show();
        break;
    }
}

QString MirrorPair::getSupportWifiDeviceText()
{
    QString link = LanguageMgr::getCurrentLanguageName() == CHINES_LANGUAGE ? SUPPORT_WIFI_DEVICES_LINK_CHINESE : SUPPORT_WIFI_DEVICES_LINK_ENGLISH;
    return QString(R"(<a style = "font-family: Source Han Sans CN Regular;color: #FF8D28;font-size: 14px;text-decoration:underline;"
                                href="%1">%2)").arg(link, LAN_WIFI_NONE_PASSWORD_HINT);
}
