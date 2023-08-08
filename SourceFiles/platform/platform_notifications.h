/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */
#pragma once

#include "base_util.h"

#include <QString>
#include "flowdrop/flowdrop.hpp"

namespace Platform {
    namespace Notifications {
        bool Supported();

        bool Init();

        void askNotification(const QString &text, Fn<void(bool)> callback);

        void infoNotificaiton(const QString& text, Fn<void()> onClick);
    } // namespace Notifications
} // namespace Platform
