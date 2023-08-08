/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */

#include "resources.h"

#include <QFontDatabase>

void loadFont(const QString& fontFile) {
    int fontId = QFontDatabase::addApplicationFont(":/Resources/fonts/" + fontFile);
    if (fontId == -1) {
        // err
    }
}

void Resources::Init() {
    loadFont("Roboto-Bold.ttf");
    loadFont("Roboto-Medium.ttf");
    loadFont("Roboto-Regular.ttf");
}
