#ifndef COMMON_H
#define COMMON_H

#include <QString>

struct WifiInfo
{
    QString name;
    QString password;
};

enum class ConnectState{
    CONNECT,
    DISCONNECT,
};

enum class Language{
    CHINESE,
    ENGLISH,
};

enum class PairState{
    INVALID,// 无效值，未初始化。
    NONE,   // 未配对。未配对任何移动设备
    PAIRED, // 已配对。已配对过移动设备
    NEW     // 新对配。配对其他移动设备
};

const QString SUNSHINE_WEB_SERVER_ADDRESS = "https://localhost:47990";
const QString SUNSHINE_WEB_DEFAULT_USERNAME = "sunshine";
const QString SUNSHINE_WEB_DEFAULT_PASSWORD = "sunshine";

const QString CHINES_LANGUAGE = "简体中文";

const QString SUPPORT_WIFI_DEVICES_LINK_ENGLISH = "https://revopedia.revopoint3d.com/en/RevoMirror/Usermanual";
const QString SUPPORT_WIFI_DEVICES_LINK_CHINESE = "https://revopedia.revopoint3d.com/zh/RevoMirror/Usermanual";

const QString USER_AGREEMENT_LINK_ENGLISH = "https://www.revopoint3d.com/pages/revo-mirror-user-agreement";
const QString USER_AGREEMENT_LINK_CHINESE = "https://www.revopoint3d.com.cn/index.php?c=category&id=114";
const QString PRIVACY_POLICY_LINK_ENGLISH = "https://www.revopoint3d.com/pages/revo-mirror-privacy-policy";
const QString PRIVACY_POLICY_LINK_CHINESE = "https://www.revopoint3d.com.cn/index.php?c=category&id=113";

const QString THIRD_MODULE_COPYRIGHT_LINK = "https://opensource.org/licenses/GPL-3.0";

// 软件升级链接
const QString SOFTWARE_SERVER_LINK = "https://api.infly3d.com/software/version/info";


// 用户手册链接
const QString USER_MANUAL_LINK_ZH = "https://revopedia.revopoint3d.com/zh/RevoMirror/Usermanual";
const QString USER_MANUAL_LINK_EN = "https://revopedia.revopoint3d.com/en/RevoMirror/Usermanual";
const QString USER_MANUAL_LINK_JA = "https://revopedia.revopoint3d.com/ja/RevoMirror/Usermanual";

const int SUNSHINE_INIT_TIMEOUT = 30 * 1000;

const int DEFAULT_MAINWINDOW_WIDTH = 1920;   //主窗口宽
const int DEFAULT_MAINWINDOW_HEIGHT = 1080;    //主窗口高

const int LOADING_CIRCLE_SIZE = 30;

const int RV_MESSAGE_BOX_FIX_WIDTH = 400;
const int RV_MESSAGE_BOX_MARGIN = 24;

const int WINDOW_W = 518;
const int WINDOW_H = 510; //405 + 65;

const int TITILE_BAR_W = WINDOW_W;
const int TITILE_BAR_H = 48;

const int CLICENT_W = WINDOW_W;
const int CLICENT_H = WINDOW_H - TITILE_BAR_H; //357


const int LOADING_W = 271;
const int LOADING_H = 257;
const int LOADING_X = (WINDOW_W - LOADING_W)/2;
const int LOADING_Y = (WINDOW_H - LOADING_H)/2;

const int WIFI_AREA_X = 0;
const int WIFI_AREA_Y = 37;
const int WIFI_AREA_W = WINDOW_W;
const int WIFI_AREA_H = 108;

const int PAIR_AREA_X = 0;
const int PAIR_AREA_Y = 145;
const int PAIR_AREA_W = WINDOW_W;
const int PAIR_AREA_H = CLICENT_H - PAIR_AREA_Y;

const int DIALOG_W = 594;
const int DIALOG_H = 576;

const int DIALOG_TITILE_BAR_W = DIALOG_W;
const int DIALOG_TITILE_BAR_H = 48;

const int DIALOG_CLICENT_X = 0;
const int DIALOG_CLICENT_Y = DIALOG_TITILE_BAR_H;
const int DIALOG_CLICENT_W = DIALOG_W;
const int DIALOG_CLICENT_H = DIALOG_H - DIALOG_TITILE_BAR_H; //357

const int DIALOG_MENULIST_X = 0;
const int DIALOG_MENULIST_Y = 0;
const int DIALOG_MENULIST_W = 172;
const int DIALOG_MENULIST_H = DIALOG_CLICENT_H;

const int DIALOG_PAGE_X = DIALOG_MENULIST_W;
const int DIALOG_PAGE_Y = 0;
const int DIALOG_PAGE_W = DIALOG_CLICENT_W - DIALOG_MENULIST_W;
const int DIALOG_PAGE_H = DIALOG_CLICENT_H;

#endif // COMMON_H
