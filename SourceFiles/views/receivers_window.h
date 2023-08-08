/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */
#pragma once

#include <QMainWindow>
#include <QEvent>
#include <QVBoxLayout>
#include <QScrollArea>
#include "flowdrop/flowdrop.hpp"

class ReceiversWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit ReceiversWindow(const QStringList& fileNames);

public slots:
    void addReceiver(const flowdrop::DeviceInfo &deviceInfo);

protected:
    void changeEvent(QEvent *event) override;

private:
    std::atomic<bool> _stopDiscover = false;
    QThread *_discoverThread;

    QStringList _fileNames;

    QVBoxLayout *_contentLayout;
};
