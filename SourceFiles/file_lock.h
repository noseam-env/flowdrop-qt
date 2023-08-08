/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */
#pragma once

#include <QtCore/QFile>

class FileLock {
public:
    FileLock();

    bool lock(QFile &file, QIODevice::OpenMode mode);
    [[nodiscard]] bool locked() const;
    void unlock();

    static constexpr auto kSkipBytes = std::ptrdiff_t(4);

    ~FileLock();

private:
    class Lock;
    struct Descriptor;
    struct LockingPid;

    static constexpr auto kLockOffset = std::ptrdiff_t(0);
    static constexpr auto kLockLimit = kSkipBytes;

    std::unique_ptr<Lock> _lock;

};
