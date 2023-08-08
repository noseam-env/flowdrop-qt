/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */

#include "single_instance.h"

#include "application.h"

#include <QRegularExpression>

class Crc32Table {
public:
    Crc32Table() {
        auto poly = std::uint32_t(0x04c11db7);
        for (auto i = 0; i != 256; ++i) {
            _data[i] = reflect(i, 8) << 24;
            for (auto j = 0; j != 8; ++j) {
                _data[i] = (_data[i] << 1) ^ (_data[i] & (1 << 31) ? poly : 0);
            }
            _data[i] = reflect(_data[i], 32);
        }
    }

    std::uint32_t operator[](int index) const {
        return _data[index];
    }

private:
    std::uint32_t reflect(std::uint32_t val, char ch) {
        auto result = std::uint32_t(0);
        for (int i = 1; i < (ch + 1); ++i) {
            if (val & 1) {
                result |= 1 << (ch - i);
            }
            val >>= 1;
        }
        return result;
    }

    std::uint32_t _data[256];

};

std::int32_t crc32(const void *data, int len) {
    static const auto kTable = Crc32Table();

    const auto buffer = static_cast<const std::uint8_t*>(data);

    auto crc = std::uint32_t(0xffffffff);
    for (auto i = 0; i != len; ++i) {
        crc = (crc >> 8) ^ kTable[(crc & 0xFF) ^ buffer[i]];
    }

    return static_cast<std::int32_t>(crc ^ 0xffffffff);
}

[[nodiscard]] QString NameForPath(
        const QString &uniqueApplicationName,
        const QString &path) {
    const auto hash = [](const QString &text) {
        return crc32(text.data(), text.size() * sizeof(QChar));
    };
    const auto ints = std::array{ hash(uniqueApplicationName), hash(path) };
    auto raw = QByteArray(ints.size() * sizeof(ints[0]), Qt::Uninitialized);
    memcpy(raw.data(), ints.data(), raw.size());
    return QString::fromLatin1(raw.toBase64(QByteArray::Base64UrlEncoding));
}

void CleanName(const QString &name) {
#ifndef Q_OS_WIN
    QFile(name).remove();
#endif // Q_OS_WIN
}

[[nodiscard]] QByteArray EncodeMessage(const QByteArray &message) {
    return message.toBase64(QByteArray::Base64UrlEncoding) + ';';
}

[[nodiscard]] std::optional<QByteArray> DecodeMessage(const QByteArray &message) {
    if (!message.endsWith(';')) {
        return std::nullopt;
    }
    return QByteArray::fromBase64(
            message.mid(0, message.size() - 1),
            QByteArray::Base64UrlEncoding);
}

// SingleInstance

SingleInstance::SingleInstance() = default;

SingleInstance::~SingleInstance() {
    clearSocket();
    clearLock();
}

void SingleInstance::start(
        const QString &uniqueApplicationName,
        const QString &path,
        Fn<void()> primary,
        Fn<void()> secondary,
        Fn<void()> fail) {
    const auto handleError = [=](QLocalSocket::LocalSocketError error) {
        clearSocket();
        QObject::connect(
                &_server,
                &QLocalServer::newConnection,
                [=] { newInstanceConnected(); });
        if (closeExisting() && _server.listen(_name)) {
            primary();
        } else {
            fail();
        }
    };

    QObject::connect(&_socket, &QLocalSocket::connected, secondary);
    QObject::connect(&_socket, &QLocalSocket::errorOccurred, handleError);
    QObject::connect(
            &_socket,
            &QLocalSocket::disconnected,
            [=] { handleError(QLocalSocket::PeerClosedError); });

    _lockFile.setFileName(path + "_single_instance.tmp");
    _name = NameForPath(uniqueApplicationName, path);
    _socket.connectToServer(_name);
}

void SingleInstance::clearSocket() {
    QObject::disconnect(
            &_socket,
            &QLocalSocket::connected,
            nullptr,
            nullptr);
    QObject::disconnect(
            &_socket,
            &QLocalSocket::disconnected,
            nullptr,
            nullptr);
    QObject::disconnect(
            &_socket,
            &QLocalSocket::errorOccurred,
            nullptr,
            nullptr);
    QObject::disconnect(
            &_socket,
            &QLocalSocket::readyRead,
            nullptr,
            nullptr);
    _socket.close();
}

void SingleInstance::clearLock() {
    if (!_lock.locked()) {
        return;
    }
    _lock.unlock();
    _lockFile.close();
    _lockFile.remove();
}

bool SingleInstance::closeExisting() {
    if (!_lock.lock(_lockFile, QIODevice::WriteOnly)) {
        return false;
    }
    CleanName(_name);
    return true;
}

void SingleInstance::send(const QByteArray &command, Fn<void()> done) {
    Expects(_socket.state() == QLocalSocket::ConnectedState);

    const auto received = std::make_shared<QByteArray>();
    const auto handleRead = [=] {
        Expects(_socket.state() == QLocalSocket::ConnectedState);

        received->append(_socket.readAll());
        if (const auto response = DecodeMessage(*received)) {
            const auto match = QRegularExpression(
                    "^PID:(\\d+);WND:(\\d+);$"
            ).match(QString::fromLatin1(*response));
            /*if (const auto pid = match.captured(1).toULongLong()) {
                Platform::ActivateProcessWindow(
                        static_cast<qint64>(pid),
                        static_cast<WId>(match.captured(2).toULongLong()));
            }*/
            done();
        }
    };
    QObject::connect(&_socket, &QLocalSocket::readyRead, handleRead);
    _socket.write(EncodeMessage(command));
}

void SingleInstance::newInstanceConnected() {
    while (const auto client = _server.nextPendingConnection()) {
        _clients.emplace(client, Message{ ++_lastMessageId });
        QObject::connect(client, &QLocalSocket::readyRead, [=] {
            readClient(client);
        });
        QObject::connect(client, &QLocalSocket::disconnected, [=] {
            removeClient(client);
        });
    }
}

void SingleInstance::readClient(not_null<QLocalSocket*> client) {
    //Expects(_clients.contains(client));
    Expects(_clients.find(client) != _clients.end());

    auto &info = _clients[client];
    info.data.append(client->readAll());
    if (const auto message = DecodeMessage(info.data)) {
        //_commands.fire({ info.id, *message });
        //_commands.push_back({ info.id, *message });
        qDebug() << "New message";
        App().openOrFocusSettings();
    }
}

void SingleInstance::removeClient(not_null<QLocalSocket*> client) {
    QObject::disconnect(client, &QLocalSocket::readyRead, nullptr, nullptr);
    QObject::disconnect(
            client,
            &QLocalSocket::disconnected,
            nullptr,
            nullptr);
    //_clients.remove(client);
    _clients.erase(client);
}

