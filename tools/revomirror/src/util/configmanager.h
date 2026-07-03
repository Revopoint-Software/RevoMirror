#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    static ConfigManager& instance() {
        static ConfigManager instance;
        return instance;
    }

    // 保存配置
    bool save();

    // 读取配置值
    QVariant getValue(const QString &key, const QVariant &defaultValue = QVariant()) const;
    QString getString(const QString &key, const QString &defaultValue = QString()) const;
    int getInt(const QString &key, int defaultValue = 0) const;
    bool getBool(const QString &key, bool defaultValue = false) const;
    double getDouble(const QString &key, double defaultValue = 0.0) const;
    QStringList getStringList(const QString &key, const QStringList &defaultValue = QStringList()) const;

    // 设置配置值
    void setValue(const QString &key, const QVariant &value);

    // 检查键是否存在
    bool contains(const QString &key) const;

    // 删除配置项
    void remove(const QString &key);

    // 获取所有键
    QStringList allKeys() const;

    // 重置到默认配置
    void resetToDefaults();

public:
    QString getLanguage();

    void setLanguage(const QString& language);
signals:
    void configChanged(const QString &key, const QVariant &value);

private:
    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager() = default;

    // 初始化配置
    bool initialize(const QString &configFileName = "config.json");

    // 禁用拷贝构造和赋值
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    // 私有方法
    void createDefaultConfig();
    QVariant getNestedValue(const QStringList &keys, const QJsonObject &obj) const;
    void setNestedValue(const QStringList &keys, const QVariant &value, QJsonObject &obj);
    bool containsNestedKey(const QStringList &keys, const QJsonObject &obj) const;
    void removeNestedKey(const QStringList &keys, QJsonObject &obj);
    void getAllKeysRecursive(const QJsonObject &obj, const QString &prefix, QStringList &keys) const;

    void setNestedValueHelper(const QStringList &keys, const QVariant &value, QJsonObject &obj);

    QString m_configFilePath;
    QJsonObject m_config;
    QJsonObject m_defaultConfig;
};

#endif // CONFIGMANAGER_H
