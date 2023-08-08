/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */

#include "qtmaterialcircularprogress_internal.h"

QtMaterialCircularProgressDelegate::QtMaterialCircularProgressDelegate(QtMaterialCircularProgress *parent)
    : QObject(parent),
      m_progress(parent),
      m_dashOffset(0),
      m_dashLength(89),
      m_angle(0)
{
    Q_ASSERT(parent);
}

QtMaterialCircularProgressDelegate::~QtMaterialCircularProgressDelegate() {}
