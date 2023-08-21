/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */

#include "application.h"

#include "knot/deviceinfo.h"
#include "platform/platform_notifications.h"
#include "platform/platform_tray.h"
#include "views/receivers_window.h"
#include "views/settings_window.h"

#include <future>
#include <gsl/gsl>
#include <QThread>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QFileDialog>
#include <QApplication>
#include <QStyleFactory>
#include "flowdrop/flowdrop.hpp"

std::optional<std::string> settingOr(const std::unique_ptr<Settings> &settings, Setting setting, std::optional<std::string> &second) {
    QString first = settings->getValue(setting);
    return !first.isEmpty() ? first.toStdString() : second.has_value() ? second : std::nullopt;
}

QString getDeviceName(const flowdrop::DeviceInfo &d) {
    std::string str = d.name.has_value() ? d.name.value() : d.model.has_value() ? d.model.value() : d.id;
    return {str.c_str()};
}

class EventListener : public flowdrop::IEventListener {
    void onReceivingEnd(const flowdrop::DeviceInfo &sender, std::uint64_t totalSize, const std::vector<flowdrop::FileInfo> &receivedFiles) override {
        QString text = "Received " + QString::number(receivedFiles.size()) + " file(s) from " + getDeviceName(sender);
        Platform::Notifications::infoNotification(text, [](){
            QString folderPath = App().getDestDir();
            QUrl folderUrl = QUrl::fromLocalFile(folderPath);
            if (!QDesktopServices::openUrl(folderUrl)) {
                qDebug() << "Failed to open folder";
            }
        });
    }
};

Application *Instance = nullptr;

Application::Application()
        : QObject(),
          _tray(std::make_unique<Platform::Tray>()),
          _settings(std::make_unique<Settings>()) {
    Instance = this;
}

Application::~Application() {
    _server->stop();

    Instance = nullptr;
}

void Application::run() {
    QApplication::setQuitOnLastWindowClosed(false);
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    _settings->load();

    KNDeviceInfo knDeviceInfo{};
    KNDeviceInfoFetch(knDeviceInfo);
    _deviceInfo.id = flowdrop::generate_md5_id();
    _deviceInfo.name = settingOr(_settings, Setting::OverrideName, knDeviceInfo.name);
    _deviceInfo.model = settingOr(_settings, Setting::OverrideModel, knDeviceInfo.model);
    _deviceInfo.platform = settingOr(_settings, Setting::OverridePlatform, knDeviceInfo.platform);
    _deviceInfo.system_version = settingOr(_settings, Setting::OverrideSystemVersion, knDeviceInfo.system_version);

    _tray->addAction("Select files and send", [this](){
        selectFilesAndSend();
    });
    _tray->addAction("Settings", [this](){
        openOrFocusSettings();
    });
    _tray->addAction("Quit " + QApplication::applicationName(), [](){
        QApplication::quit();
    });

    _server = new flowdrop::Server(_deviceInfo);
    _server->setDestDir(_settings->getValue(Setting::Dest).toStdString());
    _server->setEventListener(new EventListener);
    _server->setAskCallback([this](const flowdrop::SendAsk &sendAsk) {
        if (isAutoAccept()) return true;
        std::promise<bool> addressPromise;
        std::future<bool> addressFuture = addressPromise.get_future();
        QString text = getDeviceName(sendAsk.sender) + " would like to send you " + QString::number(sendAsk.files.size()) + " file(s)";
        Platform::Notifications::askNotification(text, [&addressPromise](bool result) {
                                                     addressPromise.set_value(result);
                                                 });
        addressFuture.wait();
        return addressFuture.get();
    });
    _serverThread = new QThread();
    QObject::connect(_serverThread, &QThread::started, [this](){
        _server->run();
    });
    _serverThread->start();

    QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, [this]() {
        _localServer.close();

        _server->stop();
        _serverThread->quit();
        _serverThread->wait();
        delete _serverThread;
        _serverThread = nullptr;
    });
}

bool Application::isAutoAccept() {
    if (!_askSupported) {
        return true;
    }
    return isAutoAcceptMode();
}

bool Application::isAutoAcceptMode() {
    return _settings->getValue(Setting::AskMode) == "AUTO";
}

void Application::setAutoAcceptMode(bool enabled) {
    _settings->setValue(Setting::AskMode, enabled ? "AUTO" : "ALWAYS");
    _settings->save();
}

void Application::setAskSupported(bool supported) {
    _askSupported = supported;
}

QString Application::getDestDir() {
    return _settings->getValue(Setting::Dest);
}

void Application::setDestDir(const QString& destDir) {
    _settings->setValue(Setting::Dest, destDir);
    _settings->save();
}

void Application::selectFilesAndSend() {
    QStringList fileNames = QFileDialog::getOpenFileNames(nullptr, "Select Files", QDir::homePath());
    if (fileNames.isEmpty()) return;
    auto *window = new ReceiversWindow(fileNames);
    window->show();
}

void Application::openOrFocusSettings() {
    SettingsWindow::openOrFocus();
}

const flowdrop::DeviceInfo &Application::deviceInfo() {
    return _deviceInfo;
}

void Application::sendTo(const QString &receiverId, const QStringList &files) {
    flowdrop::SendRequest request;
    request.setDeviceInfo(deviceInfo());
    request.setReceiverId(receiverId.toStdString());
    std::vector<flowdrop::File *> pfiles;
    foreach (const QString& filePathStr, files) {
        std::filesystem::path filePath(filePathStr.toStdString());
        pfiles.push_back(new flowdrop::NativeFile(filePath, filePath.filename().string()));
    }
    request.setFiles(pfiles);
    request.execute();
}

Application &App() {
    Expects(Instance != nullptr);
    return *Instance;
}
