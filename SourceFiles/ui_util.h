/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */

#pragma once

#include <QLabel>

class MyText : public QLabel {
public:
    explicit MyText(const QString &text, int pointSize = 13, QWidget *parent = nullptr);

    void setColor(QColor color);

    [[nodiscard]] QSize sizeHint() const override;
};

void setWidgetBackgroundColor(QWidget * widget, const QColor &color);
