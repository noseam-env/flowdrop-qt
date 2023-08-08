/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */

#include "settings.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <utility>
#include <QStandardPaths>
#include <QDir>

QString getDefaultDest() {
    QString downloadsDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QDir dir(downloadsDir);
    return dir.filePath("FlowDrop");
}

Settings::Settings() : m_version("1.0") {
    m_settingsPath = QDir::home().filePath(".flowdrop-qt.json");
    m_settingToStringMap = {
            {Setting::Dest, "dest"},
            {Setting::AskMode, "ask_mode"},
            {Setting::OverrideName, "override_name"},
            {Setting::OverrideModel, "override_model"},
            {Setting::OverridePlatform, "override_platform"},
            {Setting::OverrideSystemVersion, "override_system_version"}
    };
    m_settings[settingToString(Setting::Dest)] = getDefaultDest();
    m_settings[settingToString(Setting::AskMode)] = "ALWAYS";

    for (const auto& entry : m_settingToStringMap) {
        m_stringToSettingMap[entry.second] = entry.first;
    }
}

void Settings::setValue(Setting setting, const QString& value) {
    m_settings[settingToString(setting)] = value;
}

QString Settings::getValue(Setting setting) const {
    QString key = settingToString(setting);
    if (m_settings.contains(key)) {
        return m_settings[key].toString();
    }
    return {};
}

bool Settings::save() {
    QFile file(m_settingsPath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonObject json;
    json["version"] = m_version;

    for (auto it = m_settings.constBegin(); it != m_settings.constEnd(); ++it) {
        stringToSetting(it.key());
        json[it.key()] = it.value();
    }

    QJsonDocument doc(json);
    file.write(doc.toJson());
    file.close();
    return true;
}

bool Settings::load() {
    QFile file(m_settingsPath);
    if (!file.exists()) {
        save();
    }
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject json = doc.object();

    if (json.contains("version")) {
        m_version = json["version"].toString();
    }

    for (auto it = json.constBegin(); it != json.constEnd(); ++it) {
        if (it.key() != "version") {
            stringToSetting(it.key());
            m_settings[it.key()] = it.value().toString();
        }
    }

    return true;
}

QString Settings::settingToString(Setting setting) const {
    auto it = m_settingToStringMap.find(setting);
    if (it != m_settingToStringMap.end()) {
        return it->second;
    }
    throw std::runtime_error("Undefined setting");
}

Setting Settings::stringToSetting(const QString& str) const {
    auto it = m_stringToSettingMap.find(str);
    if (it != m_stringToSettingMap.end()) {
        return it->second;
    }
    throw std::runtime_error("Undefined setting");
}