#include "moredialog.h"
#include <QApplication>
#include <QScreen>
#include <QTextStream>
#include <QComboBox>
#include <QPainter>
#include <QDesktopServices>
#include <QDebug>
#include <QStylePainter>
#include <QStyleFactory>
#include <QMovie>

#include "src/common.h"
#include "gradientline.h"
#include "src/ui/lang/language.h"
#include "src/ui/lang/languagemgr.h"
#include "src/util/configmanager.h"
#include "src/bl/versionmanager.h"
#include "src/ui/messagedialog.h"
#include "src/ui/versiondialog.h"

QString MoreDialog::g_selectLanguage = "";

MoreDialog::MoreDialog(QWidget *parent)
    : QDialog(parent)
    , m_mainLayout(nullptr)
    , m_titleBar(nullptr)
    , m_contentWidget(nullptr)
    , m_menuList(nullptr)
    , m_pageStack(nullptr)
    , m_aboutPage(nullptr)
    , m_settingsPage(nullptr)
{
    setupUI();
}

MoreDialog::~MoreDialog()
{
}

void MoreDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        m_titleBar->setTitle(LAN_MORE_TITLE);
        m_aboutMenuItem->setText(LAN_ABOUT_MENU_TITLE);
        m_settingMenuItem->setText(LAN_OTHER_MENU_TITLE);
    }
    QDialog::changeEvent(event);
}

void MoreDialog::setSelectLanguage(const QString &language)
{
    MoreDialog::g_selectLanguage = language;
}

void MoreDialog::setupUI()
{
    resize(DIALOG_W, DIALOG_H);

    // 设置对话框属性 - 移除系统标题栏
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setModal(true);

    // 居中显示
    QScreen* screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);

    setupTitleBar();
    setupContent();

    m_menuList->setCurrentRow(0);
    //m_pageStack->setCurrentIndex(0);
}

void MoreDialog::setupTitleBar()
{
    m_titleBar = new DialogCustomTitleBar(LAN_MORE_TITLE, this);
    m_titleBar->setGeometry(0, 0, DIALOG_TITILE_BAR_W, DIALOG_TITILE_BAR_H);

    // 连接关闭信号
    connect(m_titleBar, &DialogCustomTitleBar::closeClicked,
            this, &MoreDialog::onTitleBarCloseClicked);
}

void MoreDialog::setupContent()
{
    m_contentWidget = new QWidget(this);
    m_contentWidget->setStyleSheet("background-color: none;");
    m_contentWidget->setGeometry(DIALOG_CLICENT_X, DIALOG_CLICENT_Y, DIALOG_CLICENT_W, DIALOG_CLICENT_H);

    setupMenuList();

//    QLabel *line = new QLabel(m_contentWidget);
//    line->setStyleSheet("background-color:#555555;");
//    line->setGeometry(DIALOG_MENULIST_W, 0, 2, DIALOG_CLICENT_H);  // 设置固定尺寸

    // 创建垂直渐变线条
    GradientLine *verticalLine = new GradientLine(2, DIALOG_CLICENT_H, "#434343", GradientLine::Vertical, m_contentWidget);
    verticalLine->move(DIALOG_MENULIST_W, 0);

    setupPages();
}

void MoreDialog::setupMenuList()
{
    m_menuList = new QListWidget(m_contentWidget);
    m_menuList->setSpacing(2);
    m_menuList->setFixedSize(DIALOG_MENULIST_W, DIALOG_MENULIST_H);
    m_menuList->setFrameShape(QFrame::NoFrame);
    m_menuList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //m_menuList->setItemDelegate(new NoEllipsisDelegate());

    // 添加菜单项
    m_aboutMenuItem = new QListWidgetItem(LAN_ABOUT_MENU_TITLE);
    m_menuList->addItem(m_aboutMenuItem);
    m_settingMenuItem = new QListWidgetItem(LAN_OTHER_MENU_TITLE);
    m_menuList->addItem(m_settingMenuItem);

    // 设置菜单样式，包含外边距
    m_menuList->setStyleSheet(
        "QListWidget {"
        "    width: 132px;"
        "    background-color: transparent;"
        "    color: none;"
        "    border: none;"
        "    outline: none;"
        "    margin: 8px 20px 8px 20px;"        // 外边距：上8 右20 下8 左20
        "    font-family: Source Han Sans CN Regular;"
        "    font-size: 16px;"
        "    font-weight: 400;"
        "}"
        "QListWidget::item {"
        "    background-color: transparent;"
        "    height: 48px;"                      // 菜单项高度 48px
        "    padding-left: 16px;"      // 内边距：上12 右16 下12 左16
        "    border-radius: 4px;"
        "    color: white;"
        "    font-size: 16px;"
        "    font-weight: 400;"
        "}"
        "QListWidget::item:selected {"
        "    background-color: #595959;"
        "    color: white;"
        "    font-weight: 500;"
        "}"
        "QListWidget::item:hover {"
        "    color: white;"
        "    font-weight: 400;"
        "}"
        "QListWidget::item:selected:hover {"
        "    color: white;"
        "    font-weight: 500;"
        "}"
        "QListWidget::item:focus {"
        "    outline: none;"
        "}"
    );

    // 连接信号
    connect(m_menuList, &QListWidget::currentRowChanged,
            this, &MoreDialog::onMenuItemClicked);
}

void MoreDialog::setupPages()
{
    m_pageStack = new QStackedWidget(m_contentWidget);
    m_pageStack->setGeometry(DIALOG_PAGE_X, DIALOG_PAGE_Y, DIALOG_PAGE_W, DIALOG_PAGE_H);

    // 创建页面
    m_aboutPage = new AboutPage(this);
    m_settingsPage = new SettingsPage(this);

    // 添加到堆栈
    m_pageStack->addWidget(m_aboutPage);
    m_pageStack->addWidget(m_settingsPage);
}

void MoreDialog::onMenuItemClicked(int index)
{
    if(0 == index)
    {
        m_aboutPage->reset();
    }
    m_pageStack->setCurrentIndex(index);
}

void MoreDialog::onTitleBarCloseClicked()
{
    //保存配置
    ConfigManager::instance().save();

    reject(); // 或者使用 accept() 根据需要
}

class NoShadowComboBox : public QComboBox
{
public:
    NoShadowComboBox(QWidget *parent = nullptr) : QComboBox(parent)
    {
        view()->window()->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
        view()->window()->setAttribute(Qt::WA_TranslucentBackground);
        view()->window()->setCursor(Qt::PointingHandCursor);
        QStyledItemDelegate *delegate = new QStyledItemDelegate(this);
        setItemDelegate(delegate);
    }
protected:
    void paintEvent(QPaintEvent *event) override
    {
        QStylePainter painter(this);
        painter.setPen(palette().color(QPalette::Text));

        QStyleOptionComboBox opt;
        initStyleOption(&opt);
        opt.currentIcon = QIcon();
        opt.iconSize = QSize();
        painter.drawComplexControl(QStyle::CC_ComboBox, opt);
        painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
    }
};

static int selectLanguageIndex = 0;

void ImageTextItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 调试输出（可选）

    // 正确的状态检测逻辑
    bool isSelected = index.row() == selectLanguageIndex; // (option.state & QStyle::State_Selected);
    bool isHovered = (option.state & QStyle::State_MouseOver) && !isSelected;
    bool isNormal = !isSelected && !isHovered;

//            qDebug() << "index:" << index.row()
//                     << "State flags:" << option.state
//                     << "Selected:" << isSelected
//                     << "Hovered:" << isHovered
//                     << "Normal:" << isNormal
//                     << "rect:" << option.rect;

    painter->save();

    // 获取数据
    QString text = index.data(Qt::DisplayRole).toString();
    QIcon icon = QIcon(":/resources/images/icon_pop_select.png");

    // 设置背景色
    int marginH = 2;
    int marginV = 2;

    QRect rect = option.rect;
    QRect bkgRect = QRect(rect.left() + marginH, rect.top() + marginV, rect.width() - 2*marginH, rect.height() - 2*marginV);
    QColor color("#2B2B2B");   // 默认背景色
    if (isSelected) {
        color = QColor("#E64545");  // 选中背景色
    } else if (option.state & QStyle::State_MouseOver) {
        color = QColor("#383838");  // 悬停背景色
    }

    painter->setBrush(color);
    painter->setPen(color);
    painter->drawRoundedRect(bkgRect, 2, 2);


    // 设置文本颜色
    QColor textColor = QColor("white");

    if (isSelected) {
        QRect iconRect(rect.left() + marginH + 8,rect.top() + marginV + 9,12,12);

        QPixmap pixmap = QPixmap(":/resources/images/icon_pop_select.png").scaled(12, 12, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter->drawPixmap(iconRect, pixmap);
    }


    // 绘制文本
    QRect textRect(rect.left() + marginH + 24, rect.top() + marginV + 4, rect.width() - 24, 22);

    painter->setPen(textColor);
    painter->setFont(option.font);
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);

    painter->restore();
}

// 关于页面实现
AboutPage::AboutPage(QWidget* parent) : QWidget(parent)
{
    m_dialog = static_cast<MoreDialog*>(parent);

    QVBoxLayout* layout = new QVBoxLayout(this);
    //layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignTop);

    // 应用图标和名称
    QLabel* appIcon = new QLabel(this);
    appIcon->setPixmap(QPixmap(":/resources/images/icon_logo_about.png").scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    appIcon->setStyleSheet(
        "QLabel {"
        "    margin-top: 5px;"
        "}"
    );

    QLabel* appName = new QLabel("Revo Mirror", this);
    appName->setStyleSheet(
        "QLabel {"
        "    font-family: Source Han Sans CN Regular;"
        "    margin-top: 31px;"
        "    font-size: 24px;"
        "    color: white;"
        "    font-weight: medium;"
        "}"
    );

    QLabel* version = new QLabel(QString("V%1").arg(PROJECT_VERSION), this);
    version->setStyleSheet(
        "QLabel {"
        "    font-family: Source Han Sans CN Regular;"
        "    margin-top: 10px;"
        "    font-size: 14px;"
        "    color: #666666;"
        "}"
    );

    QMovie* movie = new QMovie(":resources/images/icon_dialog_loading.gif");
    m_updateMoviceLabel = new QLabel(this);
    m_updateMoviceLabel->setAlignment(Qt::AlignCenter);
    m_updateMoviceLabel->setMovie(movie);
    m_updateMoviceLabel->hide();
    m_updateMoviceLabel->setContentsMargins(0,0,0,0);
    m_updateMoviceLabel->setStyleSheet(R"(
        QLabel{
            margin-top: 35px;
            min-height: 60px;
            max-height: 60px;
            background-color: transparent;
        })");

    m_updateBtn = new QPushButton();
    m_updateBtn->setText(LAN_CHECK_VERSION_UPDATE);
    m_updateBtn->setContentsMargins(0,0,0,0);
    connect(m_updateBtn, &QPushButton::clicked, [this](){
        checkAndUpdate();
    });
    m_updateBtn->setStyleSheet(R"(
        QPushButton {
            margin-top: 35px;
            padding-left: 30px;
            padding-right: 30px;
            min-height: 60px;
            max-height: 60px;
            font-family: Source Han Sans CN Regular;
            font-size: 20px;
            background-color: #E64545;
            color: #FFFFFF;
            border: none;
            border-radius: 4px;
        }

        QPushButton:hover {
            background-color: #F2746F;
        }

        QPushButton:pressed {
            background-color: #BF3035;
        })");

    QLabel* rightBrief1 = new QLabel("@2025.Revopoint 3D Technologies Inc.", this);
    rightBrief1->setStyleSheet(
        "QLabel {"
        "    font-family: Source Han Sans CN Regular;"
        "    margin-top: 35px;"
        "    font-size: 16px;"
        "    color: white;"
        "}"
    );

    QLabel* rightBrief2 = new QLabel("All rights reserved.", this);
    rightBrief2->setStyleSheet(
        "QLabel {"
        "    font-family: Source Han Sans CN Regular;"
        "    margin-top: 4px;"
        "    font-size: 16px;"
        "    color: white;"
        "}"
    );

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setSpacing(10);
    hLayout->setAlignment(Qt::AlignHCenter);
    hLayout->setContentsMargins(0, 24, 0, 0); // 只设置上边距35px

    m_userAgreement = new QLabel(this);
    m_userAgreement->setOpenExternalLinks(true);
    m_userAgreement->setText(getUserAgreementText());

    QLabel *line = new QLabel();
    line->setStyleSheet("background-color:#555555;");
    line->setFixedSize(2, 20);  // 设置固定尺寸

    m_privacyPolicy = new QLabel(this);
    m_privacyPolicy->setOpenExternalLinks(true);
    m_privacyPolicy->setText(getPrivacyPolicyText());


    hLayout->addWidget(m_userAgreement);
    hLayout->addWidget(line);
    hLayout->addWidget(m_privacyPolicy);


    m_thridCopyright = new QLabel(this);
    m_thridCopyright->setOpenExternalLinks(true);
    m_thridCopyright->setContentsMargins(0, 4, 0, 0); // 只设置上边距35px
    m_thridCopyright->setText(getThridCopyrightText());

    layout->addWidget(appIcon, 0, Qt::AlignHCenter);
    layout->addWidget(appName, 0, Qt::AlignHCenter);
    layout->addWidget(version, 0, Qt::AlignHCenter);
    layout->addWidget(m_updateMoviceLabel, 0, Qt::AlignHCenter);
    layout->addWidget(m_updateBtn, 0, Qt::AlignHCenter);
    layout->addWidget(rightBrief1, 0, Qt::AlignHCenter);
    layout->addWidget(rightBrief2, 0, Qt::AlignHCenter);
    layout->addLayout(hLayout);
    layout->addWidget(m_thridCopyright, 0, Qt::AlignHCenter);
}

void AboutPage::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        m_updateBtn->setText(LAN_CHECK_VERSION_UPDATE);
        m_thridCopyright->setText(getThridCopyrightText());
        m_privacyPolicy->setText(getPrivacyPolicyText());
        m_userAgreement->setText(getUserAgreementText());
    }
    QWidget::changeEvent(event);
}

QString AboutPage::getUserAgreementText()
{
    QString link = LanguageMgr::getCurrentLanguageName() == CHINES_LANGUAGE ? USER_AGREEMENT_LINK_CHINESE : USER_AGREEMENT_LINK_ENGLISH;
    return QString(R"(<a style = "font-family: Source Han Sans CN Regular;color: #E64545;font-size: 16px;text-decoration: none;"
                            href="%1">%2)").arg(link, LAN_USER_AGREEMENT);
}

QString AboutPage::getPrivacyPolicyText()
{
    QString link = LanguageMgr::getCurrentLanguageName() == CHINES_LANGUAGE ? PRIVACY_POLICY_LINK_CHINESE : PRIVACY_POLICY_LINK_ENGLISH;
    return QString(R"(<a style = "font-family: Source Han Sans CN Regular;color: #E64545;font-size: 16px;text-decoration: none;"
                            href="%1">%2)").arg(link, LAN_PRIVACY_POLICY);
}

QString AboutPage::getThridCopyrightText()
{
    return QString(R"(<a style = "font-family: Source Han Sans CN Regular;color: #E64545;font-size: 16px;text-decoration: none;"
                            href="%1">%2)").arg(THIRD_MODULE_COPYRIGHT_LINK, LAN_OPEN_SOURCE_LICENSE);
}

void AboutPage::checkAndUpdate()
{
    m_updateBtn->hide();
    m_updateMoviceLabel->movie()->start();
    m_updateMoviceLabel->show();

    VersionManager::instance().syncCheckLatestVersion();

    if(!VersionManager::instance().checkVersionResult() && VersionManager::instance().latestVersion().isEmpty())
    {
        MessageDialog::information(LAN_DETECTION_FAILURE, LAN_NOTNETWORK_TIP, m_dialog);
    }
    else
    {
        if (!VersionManager::instance().isLatestVersion()) {
            VersionConfig verConfig;
            verConfig.manual = true;
            verConfig.url = VersionManager::instance().downloadUrl();
            verConfig.description = VersionManager::instance().versionDesc();
            verConfig.version = VersionManager::instance().latestVersion();
            VersionDialog versionDialog(verConfig, m_dialog);
            versionDialog.exec();
        }
        else if (VersionManager::instance().isLatestVersion()) {
            m_updateBtn->setText(LAN_ALREADY_VERSION);
            m_updateBtn->setStyleSheet(m_updateBtn->styleSheet() +  "QPushButton {background: #333333;}");
            m_updateBtn->setEnabled(false);
        }
    }

    m_updateMoviceLabel->movie()->stop();
    m_updateMoviceLabel->hide();
    m_updateBtn->show();
}

void AboutPage::reset()
{
  m_updateBtn->setText(LAN_CHECK_VERSION_UPDATE);
  m_updateBtn->setStyleSheet(m_updateBtn->styleSheet() +  "QPushButton {background: #E64545;}");
  m_updateBtn->setEnabled(true);
}

// 设置页面实现
SettingsPage::SettingsPage(QWidget* parent) : QWidget(parent)
{
    m_dialog = static_cast<MoreDialog*>(parent);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(20, 22, 20, 22); // 只设置上边距35px

    m_languageLabel = new QLabel(LAN_LANGUAGE, this);
    m_languageLabel->setAlignment(Qt::AlignVCenter);
    m_languageLabel->setStyleSheet(
        "QLabel {"
        "    font-family: Source Han Sans CN Regular;"
        "    font-size: 16px;"
        "    color: #A6A6A6;"
        "}"
    );

    // 创建 ComboBox
    m_langComboBox = new NoShadowComboBox();
#ifdef __APPLE__
    // 这行代码的作用是：
    // 1、绕过Mac系统样式：Mac的原生样式会添加系统级的阴影和边框效果
    // 2、使用Windows样式：Windows样式更简洁，不会添加额外的视觉效果
    // 3、保持跨平台一致性：确保在不同操作系统上的显示效果一致
    m_langComboBox->setStyle(QStyleFactory::create("Windows"));
  #endif
    // 添加选项
    QMap<QString, QString> langs = LanguageMgr::getLanguage();
    QMapIterator<QString, QString> i(langs);
    while (i.hasNext()) {
        i.next();
        m_langComboBox->addItem(i.value(), i.key());
    }

    m_langComboBox->setMaxVisibleItems(langs.count());


    // 创建自定义ListView
    QListView *listView = new QListView(m_langComboBox);
    m_langComboBox->setView(listView);

    // 设置自定义委托
    ImageTextItemDelegate *delegate = new ImageTextItemDelegate(this);
    listView->setItemDelegate(delegate);


    // 设置样式
    m_langComboBox->setStyleSheet(
        "QComboBox {"
        "    margin-top: 12px;"
        "    height: 24px;"
        "    min-height: 24px;"
        "    max-height: 24px;"
        "    font-family: Source Han Sans CN Regular;"
        "    border-radius: 4px;"
        "    padding: 8px 12px;"
        "    background-color: #2B2B2B;"
        "    font-size: 16px;"
        "    color: white;"
        "}"
        "QComboBox::drop-down {"
        "    width: 20px;"
        "    border: none;"
        "}"
        "QComboBox::down-arrow {"
        "    image: url(:/resources/images/icon_down_arrow.png);"  // 自定义下拉箭头
        "    padding-right:14px;"
        "    width: 16px;"
        "    height: 16px;"
        "}"

        // 对ListView设置样式
        "QListView {"
        "    font-family: Source Han Sans CN Regular;"
        "    font-size: 14px;"
        "    border: none;"
        "    border-radius: 4px;"
        "    background-color: #2B2B2B;"
        "    margin-top:4px;"
        "    outline: none;"
        "    padding: 4px;"
        "}"

        // ListView的item样式
        "QListView::item {"
        "    height: 34px;"
        "    border: none;"
        "    border-radius: 2px;"
        "    color: white;"
        "}"
    );

    QString language = ConfigManager::instance().getLanguage();

    int index = m_langComboBox->findData(language);
    qInfo() << "init language " << language << ", index " << index;
    m_langComboBox->setCurrentIndex(index);
    selectLanguageIndex = index;
    if(m_dialog)
    {
        connect(m_langComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), m_dialog, &MoreDialog::signalSelectLanguage);
        connect(m_langComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
            selectLanguageIndex = index;

            QString data = m_langComboBox->itemData(index).toString();

            MoreDialog::setSelectLanguage(data);

            LanguageMgr::switchLanguage(data);


            ConfigManager::instance().setLanguage(data);
        });
    }

    layout->addWidget(m_languageLabel);

    layout->addWidget(m_langComboBox);
}

void SettingsPage::changeEvent(QEvent *event)
{
    m_languageLabel->setText(LAN_LANGUAGE);
}

