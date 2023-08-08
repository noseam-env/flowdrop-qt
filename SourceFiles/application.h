/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */
#pragma once

#include <QLocalServer>
#include "QObject"
#include "platform/platform_tray.h"
#include "settings.h"

class Application final : public QObject {
public:
    Application();
    ~Application() override;

    void run();

    bool isAutoAcceptMode();

    void setAutoAcceptMode(bool enabled);

    QString getDestDir();

    void setDestDir(const QString& destDir);

    void selectFilesAndSend();

    void openOrFocusSettings();

    void sendTo(const QString &receiverId, const QStringList &files);

    const flowdrop::DeviceInfo &deviceInfo();

private:
    const std::unique_ptr<Platform::Tray> _tray;
    const std::unique_ptr<Settings> _settings;
    QString _localServerName;
    QLocalServer _localServer;
    flowdrop::DeviceInfo _deviceInfo;
    flowdrop::Server *_server;
    QThread *_serverThread = nullptr;
    bool _autoAcceptMode = false;
};

[[nodiscard]] Application &App();
