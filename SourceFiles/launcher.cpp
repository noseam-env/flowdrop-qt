/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */

#include "launcher.h"

#include "platform/platform_notifications.h"
#include "resources.h"
#include "application.h"
#include "single_instance.h"

#include <QDir>
#include <QFile>
#include <QDebug>
#include <QProcess>
#include <QMessageBox>
#include <QStyleHints>
#include <QApplication>
#include <iostream>

void logToFile(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QFile file("flowdrop-qt.log");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        qDebug() << "Ошибка открытия файла для записи.";
        return;
    }

    QTextStream stream(&file);

    switch (type) {
        case QtDebugMsg:
            stream << "[Debug]: ";
            break;
        case QtInfoMsg:
            stream << "[ Info]: ";
            break;
        case QtWarningMsg:
            stream << "[ Warn]: ";
            break;
        case QtCriticalMsg:
            stream << "[ Crit]: ";
            break;
        case QtFatalMsg:
            stream << "[Fatal]: ";
            break;
    }

    stream << msg << "\n";
}

void initQtMessageLogging() {
    static QtMessageHandler OriginalMessageHandler = nullptr;
    OriginalMessageHandler = qInstallMessageHandler([](
            QtMsgType type,
            const QMessageLogContext &context,
            const QString &msg) {
        if (OriginalMessageHandler) {
            OriginalMessageHandler(type, context, msg);
        }
        logToFile(type, context, msg);
    });
}

void continueLaunch() {
    Resources::Init();

    bool askSupported = false;
    auto supportLevel = Platform::Notifications::Supported();
    if (supportLevel != Platform::Notifications::NOT_SUPPORTED && Platform::Notifications::Init()) {
        askSupported = supportLevel == Platform::Notifications::FULL;
    }
    if (askSupported) {
        qInfo() << "Action notifications not supported, using auto accept mode";
    }

    auto *application = new Application();
    application->setAskSupported(askSupported);
    application->run();
}

int launch(int argc, char *argv[]) {
    QApplication::setApplicationName("FlowDrop Qt");
    QApplication::setApplicationVersion(AppVersionStr);

    initQtMessageLogging();

    QApplication app(argc, argv);

    SingleInstance singleInstance;

#ifdef __linux__
    continueLaunch();
#else

    auto onPrimaryInstance = []() {
        qInfo() << "Instance: primary";
        continueLaunch();
    };

    auto onSecondaryInstance = [&singleInstance]() {
        qInfo() << "Instance: secondary";

        singleInstance.send(QString("str").toUtf8(), [](){});

        QApplication::quit();
    };

    auto onFailInstance = []() {
        qInfo() << "Instance: fail";
        QMessageBox::critical(nullptr, "Cannot run FlowDrop Qt", "Report flowdrop-qt.log on GitHub Issues\nhttps://github.com/noseam-env/flowdrop-qt/issues");

        QApplication::quit();
    };

    QString tempPath = QDir::tempPath() + "/flowdrop-qt";
    singleInstance.start("flowdrop-qt", tempPath, onPrimaryInstance, onSecondaryInstance, onFailInstance);
#endif

    int result = app.exec();

    qInfo() << "FlowDrop Qt finised, result: " << result;

    //Platform::finish();

    return result;
}
