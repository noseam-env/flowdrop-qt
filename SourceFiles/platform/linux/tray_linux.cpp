/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */

#include "platform/platform_tray.h"

#include "application.h"
#include "style.h"

#include <QMenu>
#include <QScreen>
#include <QVBoxLayout>
#include <QStyleHints>
#include <QApplication>
#include <QStyleFactory>
#include <QScopedPointer>
#include <QSystemTrayIcon>
#include <memory>

#define TRAY_ICON_SIZE 16

namespace Platform {

    struct Tray::Impl {
        QScopedPointer<QSystemTrayIcon> _icon;
        QScopedPointer<QMenu> _menu;

        void renderAndSetIcon(bool dark) const {
            QColor color = dark ? QColorConstants::White : QColorConstants::Black;
            QIcon icon = renderIcon(TRAY_ICON_SIZE, color);
            _icon->setIcon(icon);
        }

        void themeChanged(Qt::ColorScheme colorScheme) const {
            /*std::optional<bool> darkMode;
            if (colorScheme != Qt::ColorScheme::Unknown) {
                darkMode = (colorScheme == Qt::ColorScheme::Dark);
            }
            renderAndSetIcon(darkMode.has_value() && darkMode.value());*/
            renderAndSetIcon(colorScheme == Qt::ColorScheme::Dark);
        }
    };

    Tray::Tray() : pImpl(new Impl) {
        pImpl->_icon.reset(new QSystemTrayIcon(nullptr));
        pImpl->_icon->setToolTip(QApplication::applicationName());
        pImpl->_menu.reset(new QMenu(nullptr));
        pImpl->_icon->setContextMenu(pImpl->_menu.get());

        updateIcon();

        /*QObject::connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged,
                         [this](Qt::ColorScheme colorScheme) {
                             pImpl->themeChanged(colorScheme);
                         });*/

        pImpl->_icon->show();
    }

    Tray::~Tray() = default;

    void Tray::updateIcon() {
        /*QImage finalIcon(16, 16, QImage::Format_ARGB32);
        finalIcon.fill(Qt::transparent);

        QColor color = style::logoColor;
        QPixmap icon = ::renderIcon(16, color);

        int x = (finalIcon.width() - icon.width()) / 2;
        int y = (finalIcon.height() - icon.height()) / 2;

        QPainter painter(&finalIcon);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.drawPixmap(x, y, icon);
        painter.end();

        pImpl->_icon->setIcon(QIcon(QPixmap::fromImage(finalIcon)));*/

        QColor color = style::logoColor;
        QIcon icon = renderIcon(16, color);
        pImpl->_icon->setIcon(icon);

        //pImpl->themeChanged(QGuiApplication::styleHints()->colorScheme());
    }

    void Tray::addAction(const QString &text, Fn<void()> &&callback) {
        if (pImpl->_menu) {
            pImpl->_menu->addAction(text, std::move(callback));
        }
    }

} // namespace Platform
