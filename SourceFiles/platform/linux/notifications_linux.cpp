/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */

#include "platform/platform_notifications.h"

#include <QDebug>
#undef signals
#include <libnotify/notify.h>
#include <glib.h>

namespace Platform::Notifications {
    SupportLevel Supported() {
        return TEXT_ONLY;
    }

    bool Init() {
        if (!notify_init("FlowDrop")) {
            qCritical() << "Failed to initialize libnotify";
            return false;
        }
        return true;
    }

    void askNotification(const QString& text, Fn<void(bool)> callback) {
        callback(true);
    }

    void infoNotification(const QString& text, Fn<void()> onClick) {
        NotifyNotification *notification = notify_notification_new(
                "FlowDrop",
                text.toStdString().c_str(),
                nullptr
        );

        GError *error = nullptr;
        if (!notify_notification_show(notification, &error)) {
            qCritical() << "Failed to show notification: " << error->message;
            g_error_free(error);
            return;
        }

        g_object_unref(G_OBJECT(notification));
    }
} // namespace Platform::Notifications
