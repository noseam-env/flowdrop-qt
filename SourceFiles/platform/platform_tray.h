/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */
#pragma once

#include "base_util.h"
#include "icon_util.h"
#include "resources.h"

#include <memory>
#include <QIcon>
#include <QColor>
#include <QPainter>
#include <QSvgRenderer>
#include "flowdrop/flowdrop.hpp"

namespace Platform {

    class Tray {
    public:
        Tray();
        ~Tray();

        void updateIcon();

        void addAction(const QString &text, Fn<void()> &&callback);

        static QIcon renderIcon(int size, QColor color) {
            QIcon icon(::renderIcon(size, color));
            return icon;
        }

    private:
        class Impl;
        std::unique_ptr<Impl> pImpl;
    }; // Tray

} // namespace Platform
