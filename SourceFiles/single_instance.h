/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */
#pragma once

#include "file_lock.h"

#include <base_util.h>

#include <QLocalServer>
#include <QLocalSocket>

class SingleInstance final {
public:
    struct Message {
        quint32 id = 0;
        QByteArray data;
    };

    SingleInstance();
    ~SingleInstance();

    void start(
            const QString &uniqueApplicationName,
            const QString &path,
            Fn<void()> primary,
            Fn<void()> secondary,
            Fn<void()> fail);

    void send(const QByteArray &command, Fn<void()> done);

private:
    void clearSocket();
    void clearLock();
    [[nodiscard]] bool closeExisting();

    void newInstanceConnected();
    void readClient(not_null<QLocalSocket*> client);
    void removeClient(not_null<QLocalSocket*> client);

    QString _name;
    QLocalServer _server;
    QLocalSocket _socket;
    quint32 _lastMessageId = 0;
    std::map<not_null<QLocalSocket*>, Message> _clients;
    std::vector<Message> _commands;

    QFile _lockFile;
    FileLock _lock;
};
