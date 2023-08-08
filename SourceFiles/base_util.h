/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */
#pragma once

#include <functional>
#include <gsl/gsl>

using gsl::not_null;

template <typename Signature>
using Fn = std::function<Signature>;
