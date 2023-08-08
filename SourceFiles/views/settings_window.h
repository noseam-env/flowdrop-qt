/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */
#pragma once

#include <QMainWindow>
#include <QEvent>

class SettingsWindow : public QMainWindow {
public:
    static SettingsWindow *getInstance() {
        if (!_instance) {
            _instance = new SettingsWindow;
        }
        return _instance;
    }

    static void openOrFocus() {
        SettingsWindow *instance = getInstance();
        if (instance->isHidden()) {
            instance->showNormal();
        } else {
            if (instance->isMinimized()) {
                instance->showNormal();
            }
            instance->activateWindow();
        }
        /*if (instance->isHidden()) {
            instance->show();
        } else {
            //bool minimized = instance->isMinimized();
            instance->activateWindow();
            if (instance->isMinimized()) {
                instance->show();
            }
        }*/
    }

    SettingsWindow();

protected:
    void changeEvent(QEvent *event) override;

private:
    static SettingsWindow *_instance;
};
