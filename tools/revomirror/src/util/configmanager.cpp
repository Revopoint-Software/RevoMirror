#include "configmanager.h"

ConfigManager::ConfigManager(QObject *parent) : QObject(parent)
{
    // 创建默认配置
    createDefaultConfig();

    initialize();
}

bool ConfigManager::initialize(const QString &configFileName)
{
    // 确定配置文件路径
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(configDir);
    m_configFilePath = QDir(configDir).absoluteFilePath(configFileName);

    // 尝试加载现有配置
    QFile file(m_configFilePath);
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data, &error);

        if (error.error == QJsonParseError::NoError && doc.isObject()) {
            m_config = doc.object();
            qDebug() << "Configuration loaded from:" << m_configFilePath;
            return true;
        } else {
            qWarning() << "Failed to parse config file:" << error.errorString();
        }
    }

    // 如果加载失败，使用默认配置
    m_config = m_defaultConfig;
    return save(); // 保存默认配置
}

bool ConfigManager::save()
{
    QFile file(m_configFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot open config file for writing:" << m_configFilePath;
        return false;
    }

    QJsonDocument doc(m_config);
    file.write(doc.toJson());
    file.close();

    qDebug() << "Configuration saved to:" << m_configFilePath;
    return true;
}

QVariant ConfigManager::getValue(const QString &key, const QVariant &defaultValue) const
{
    QStringList keys = key.split('.');
    QVariant value = getNestedValue(keys, m_config);
    return value.isValid() ? value : defaultValue;
}

QString ConfigManager::getString(const QString &key, const QString &defaultValue) const
{
    return getValue(key, defaultValue).toString();
}

int ConfigManager::getInt(const QString &key, int defaultValue) const
{
    return getValue(key, defaultValue).toInt();
}

bool ConfigManager::getBool(const QString &key, bool defaultValue) const
{
    return getValue(key, defaultValue).toBool();
}

double ConfigManager::getDouble(const QString &key, double defaultValue) const
{
    return getValue(key, defaultValue).toDouble();
}

QStringList ConfigManager::getStringList(const QString &key, const QStringList &defaultValue) const
{
    QVariant value = getValue(key);
    if (value.isValid() && value.canConvert<QStringList>()) {
        return value.toStringList();
    }
    return defaultValue;
}

void ConfigManager::setValue(const QString &key, const QVariant &value)
{
    QStringList keys = key.split('.');
    setNestedValue(keys, value, m_config);
    emit configChanged(key, value);
}

bool ConfigManager::contains(const QString &key) const
{
    QStringList keys = key.split('.');
    return containsNestedKey(keys, m_config);
}

void ConfigManager::remove(const QString &key)
{
    QStringList keys = key.split('.');
    removeNestedKey(keys, m_config);
}

QStringList ConfigManager::allKeys() const
{
    QStringList keys;
    getAllKeysRecursive(m_config, "", keys);
    return keys;
}

void ConfigManager::resetToDefaults()
{
    m_config = m_defaultConfig;
    save();
}

QString ConfigManager::getLanguage()
{
    return getString("application.language", "Auto");
}

void ConfigManager::setLanguage(const QString &language)
{
    setValue("application.language", language);
}

void ConfigManager::createDefaultConfig()
{
    // 创建默认配置结构
    QJsonObject app;
    app["version"] = "1.0.0";
    app["language"] = "Auto";

    m_defaultConfig["application"] = app;
}

QVariant ConfigManager::getNestedValue(const QStringList &keys, const QJsonObject &obj) const
{
    if (keys.isEmpty()) {
        return QVariant();
    }

    QJsonObject currentObj = obj;
    for (int i = 0; i < keys.size() - 1; ++i) {
        if (!currentObj.contains(keys[i]) || !currentObj[keys[i]].isObject()) {
            return QVariant();
        }
        currentObj = currentObj[keys[i]].toObject();
    }

    const QString &lastKey = keys.last();
    if (!currentObj.contains(lastKey)) {
        return QVariant();
    }

    const QJsonValue &value = currentObj[lastKey];
    return value.toVariant();
}

void ConfigManager::setNestedValue(const QStringList &keys, const QVariant &value, QJsonObject &obj)
{
    if (keys.isEmpty()) {
        return;
    }

    setNestedValueHelper(keys, value, obj);
}

bool ConfigManager::containsNestedKey(const QStringList &keys, const QJsonObject &obj) const
{
    if (keys.isEmpty()) {
        return false;
    }

    QJsonObject currentObj = obj;
    for (int i = 0; i < keys.size() - 1; ++i) {
        if (!currentObj.contains(keys[i]) || !currentObj[keys[i]].isObject()) {
            return false;
        }
        currentObj = currentObj[keys[i]].toObject();
    }

    return currentObj.contains(keys.last());
}

void ConfigManager::removeNestedKey(const QStringList &keys, QJsonObject &obj)
{
    if (keys.isEmpty()) {
        return;
    }

    if (keys.size() == 1) {
        obj.remove(keys.first());
        return;
    }

    QJsonObject *currentObj = &obj;
    QList<QJsonObject*> path;
    QStringList keyPath;

    for (int i = 0; i < keys.size() - 1; ++i) {
        const QString &key = keys[i];
        if (!currentObj->contains(key) || !(*currentObj)[key].isObject()) {
            return; // 路径不存在
        }

        path.append(currentObj);
        keyPath.append(key);

        QJsonValue val = (*currentObj)[key];
        QJsonObject childObj = val.toObject();
        currentObj = &childObj;
    }

    currentObj->remove(keys.last());

    // 重新构建路径
    for (int i = path.size() - 1; i >= 0; --i) {
        (*path[i])[keyPath[i]] = *currentObj;
        currentObj = path[i];
    }
}

void ConfigManager::getAllKeysRecursive(const QJsonObject &obj, const QString &prefix, QStringList &keys) const
{
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        QString currentKey = prefix.isEmpty() ? it.key() : prefix + "." + it.key();

        if (it.value().isObject()) {
            getAllKeysRecursive(it.value().toObject(), currentKey, keys);
        } else {
            keys.append(currentKey);
        }
    }
}

void ConfigManager::setNestedValueHelper(const QStringList &keys, const QVariant &value, QJsonObject &obj)
{
    if (keys.size() == 1) {
            // 基础情况：只有一个键
            obj[keys.first()] = QJsonValue::fromVariant(value);
            return;
        }

        // 递归情况：多个键
        const QString &firstKey = keys.first();
        QStringList remainingKeys = keys.mid(1);

        // 获取或创建子对象
        QJsonObject childObj;
        if (obj.contains(firstKey) && obj[firstKey].isObject()) {
            childObj = obj[firstKey].toObject();
        }

        // 递归设置剩余路径
        setNestedValueHelper(remainingKeys, value, childObj);

        // 将修改后的子对象设置回父对象
        obj[firstKey] = childObj;
}
