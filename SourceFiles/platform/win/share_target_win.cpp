/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */

#include "../platform_share_target.h"

#include "Windows.h"

namespace Platform::ShareTarget {
    void Init() {
        /*HKEY hKey = nullptr;
        LONG lRes = RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Classes\\YourAppName"),
                                   0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr);*/
    }
} // namespace Platform::ShareTarget
