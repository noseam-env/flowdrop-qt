/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */

#pragma once

#include <QString>
#include <QJsonObject>

enum class Setting {
    Dest,
    AskMode,
    OverrideName,
    OverrideModel,
    OverridePlatform,
    OverrideSystemVersion
};

class Settings {
public:
    Settings();

    void setValue(Setting setting, const QString& value);

    [[nodiscard]] QString getValue(Setting setting) const;

    bool save();

    bool load();

private:
    QString m_settingsPath;
    QString m_version;
    QJsonObject m_settings;
    std::unordered_map<Setting, QString> m_settingToStringMap;
    std::unordered_map<QString, Setting> m_stringToSettingMap;

    QString settingToString(Setting setting) const;

    Setting stringToSetting(const QString &str) const;
}; // Settings
