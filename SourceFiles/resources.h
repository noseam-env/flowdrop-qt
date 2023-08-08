/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */
#pragma once

#include <QString>

namespace Resources {
    static const QString &appIcon = ":/Resources/app_icon.svg";

    namespace icons {
        static const QString &coffee = ":/Resources/icons/coffee.svg";
        static const QString &device_android = ":/Resources/icons/device_android.svg";
        static const QString &device_ios = ":/Resources/icons/device_ios.svg";
        static const QString &device_mac = ":/Resources/icons/device_mac.svg";
        static const QString &device_pc = ":/Resources/icons/device_pc.svg";
    }

    void Init();
}
