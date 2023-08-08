/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */
#pragma once

#include <QtGlobal>
#include <QColor>

class QtMaterialCircularProgress;
class QtMaterialCircularProgressDelegate;

class QtMaterialCircularProgressPrivate
{
    Q_DISABLE_COPY(QtMaterialCircularProgressPrivate)
    Q_DECLARE_PUBLIC(QtMaterialCircularProgress)

public:
    QtMaterialCircularProgressPrivate(QtMaterialCircularProgress *q);
    ~QtMaterialCircularProgressPrivate();

    void init();

    QtMaterialCircularProgress         *const q_ptr;
    QtMaterialCircularProgressDelegate *delegate;
    //Material::ProgressType              progressType;
    QColor                              color;
    qreal                               penWidth;
    int                                 size;
    bool                                useThemeColors;
};
