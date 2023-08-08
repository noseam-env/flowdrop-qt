/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */

#include "ui_util.h"

MyText::MyText(const QString &text, int pointSize, QWidget *parent) : QLabel(text, parent) {
    setFont(QFont("Roboto", pointSize, QFont::Normal, false));

    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setSizePolicy(sizePolicy);
}

void MyText::setColor(QColor color) {
    QPalette labelPalette = palette();
    labelPalette.setColor(QPalette::WindowText, color);
    setPalette(labelPalette);
}

QSize MyText::sizeHint() const {
    QFontMetrics fm(font());
    QSize textSize = fm.size(Qt::TextSingleLine, text());
    return textSize;
}

void setWidgetBackgroundColor(QWidget * widget, const QColor &color) {
    QPalette palette = QPalette(widget->palette());
    palette.setColor(QPalette::Window, color);
    widget->setAutoFillBackground(true);
    widget->setPalette(palette);
}
