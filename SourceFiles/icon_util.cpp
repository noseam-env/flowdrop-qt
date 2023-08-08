/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */

#include <QSvgRenderer>
#include <QPainter>
#include "icon_util.h"
#include "resources.h"
#include "style.h"

QPixmap renderIcon(int size, QColor color) {
    QSvgRenderer renderer(Resources::appIcon);

    QImage image(size, size, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    renderer.render(&painter);

    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(image.rect(), color);

    return QPixmap::fromImage(image);
}

static std::optional<QIcon> windowIcon;

QIcon getWindowIcon() {
    if (windowIcon.has_value()) {
        return windowIcon.value();
    }
    QIcon icon;
    icon.addPixmap(renderIcon(16, style::logoColor));
    icon.addPixmap(renderIcon(32, style::logoColor));
    icon.addPixmap(renderIcon(48, style::logoColor));
    icon.addPixmap(renderIcon(128, style::logoColor));
    windowIcon = icon;
    return icon;
}
