/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */

#include "platform/platform_notifications.h"

#include "application.h"

#include <QString>
#include <QApplication>
#include "wintoastlib.h"

using namespace WinToastLib;

class AskToastHandler : public IWinToastHandler {
public:
    explicit AskToastHandler(Fn<void(bool)> callback) : _callback(std::move(callback)) {};

    void toastActivated() const override {
        _callback(true);
    }

    void toastActivated(int actionIndex) const override {
        if (actionIndex == 0) { // Decline
            _callback(false);
        } else if (actionIndex == 1) { // Accept
            _callback(true);
        }
    }

    void toastDismissed(WinToastDismissalReason state) const override {
        _callback(false);
    }

    void toastFailed() const override {
        _callback(false);
    }

private:
    Fn<void(bool)> _callback;
}; // AskToastHandler

class InfoToastHandler : public IWinToastHandler {
public:
    explicit InfoToastHandler(Fn<void()> onClick) : _onClick(onClick) {}

    void toastActivated() const override {
        _onClick();
    }
    void toastActivated(int actionIndex) const override {

    }
    void toastDismissed(WinToastDismissalReason state) const override {

    }
    void toastFailed() const override {

    }

private:
    Fn<void()> _onClick;
}; // EmptyToastHandler

/*std::wstring to_wstring(const std::string &narrowStr) {
    if (narrowStr.empty())
        return L"";

    int wideStrLength = MultiByteToWideChar(CP_UTF8, 0, narrowStr.c_str(), static_cast<int>(narrowStr.length()),
                                            nullptr, 0);
    if (wideStrLength == 0)
        return L"";

    std::wstring wideStr(wideStrLength, L'\0');
    if (MultiByteToWideChar(CP_UTF8, 0, narrowStr.c_str(), static_cast<int>(narrowStr.length()), &wideStr[0],
                            wideStrLength) == 0)
        return L"";

    return wideStr;
}*/

namespace Platform::Notifications {
    bool Supported() {
        return WinToast::isCompatible();
    }

    bool Init() {
        if (!Supported()) {
            return false;
        }
        WinToast::instance()->setAppName(QApplication::applicationName().toStdWString());
        const auto aumi = WinToast::configureAUMI(L"noseam", L"flowdrop", L"flowdropqt", L"20161006");
        WinToast::instance()->setAppUserModelId(aumi);
        if (!WinToast::instance()->initialize()) {
            std::cerr << "Error, could not initialize the lib!" << std::endl;
        }
        return true;
    }

    void askNotification(const QString& text, Fn<void(bool)> callback) {
        WinToastTemplate templ = WinToastTemplate(WinToastTemplate::Text01);
        templ.setTextField(text.toStdWString(), WinToastTemplate::FirstLine);

        std::vector<std::wstring> actions;
        actions.emplace_back(L"Decline");
        actions.emplace_back(L"Accept");

        for (auto const &action: actions) {
            templ.addAction(action);
        }
        auto *handler = new AskToastHandler(std::move(callback));
        const auto toast_id = WinToast::instance()->showToast(templ, handler);
        if (toast_id < 0) {
            std::cerr << "Error: Could not launch your toast notification!" << std::endl;
        }
    }

    void infoNotificaiton(const QString& text, Fn<void()> onClick) {
        WinToastTemplate templ = WinToastTemplate(WinToastTemplate::Text01);
        templ.setTextField(text.toStdWString(), WinToastTemplate::FirstLine);

        const auto toast_id = WinToast::instance()->showToast(templ, new InfoToastHandler(onClick));
        if (toast_id < 0) {
            std::cerr << "Error: Could not launch your toast notification!" << std::endl;
        }
    }
} // namespace Platform::Notifications
