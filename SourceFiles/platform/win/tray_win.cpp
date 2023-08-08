/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */

#include "platform/platform_tray.h"
#include <QMenu>
#include <QObject>
#include <QVBoxLayout>
#include <QStyleHints>
#include <QApplication>
#include <QStyleFactory>
#include <QScopedPointer>
#include <QSystemTrayIcon>
#include <QGraphicsView>
#include <QStyleOption>
#include <QGraphicsProxyWidget>
#include <QPropertyAnimation>
#include <QWindow>
#include <memory>
#include <Windows.h>
#include "application.h"
#include "style.h"
#include "../../ui_util.h"


class MenuItem : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)

public:
    MenuItem(const QString& text, QWidget* parent = nullptr) : QWidget(parent), _text(text) {
        setCursor(Qt::PointingHandCursor);

        QHBoxLayout* layout = new QHBoxLayout(this);
        layout->setContentsMargins(8, 8, 8, 8);

        _textLabel = new MyText(text, 11, this);
        _textLabel->setColor(style::trayText);
        layout->addWidget(_textLabel);

        _backgroundColor = style::trayBg;
        _animation = new QPropertyAnimation(this, "backgroundColor", this);
        _animation->setDuration(100);

        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setSizePolicy(sizePolicy);
    }

    QSize sizeHint() const override {
        return _textLabel->sizeHint() + QSize(16, 16);
    }

    [[nodiscard]] QColor backgroundColor() const {
        return _backgroundColor;
    }

    void setBackgroundColor(const QColor &color) {
        _backgroundColor = color;
        update();
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event)
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(rect(), _backgroundColor);
    }

    void enterEvent(QEnterEvent *event) override {
        Q_UNUSED(event)
        _animation->setStartValue(_backgroundColor);
        _animation->setEndValue(style::trayBgHovered);
        _animation->start();
    }

    void leaveEvent(QEvent *event) override {
        Q_UNUSED(event)
        _animation->setStartValue(_backgroundColor);
        _animation->setEndValue(style::trayBg);
        _animation->start();
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            emit clicked();
        }
        QWidget::mouseReleaseEvent(event);
    }

signals:
    void clicked();

private:
    QString _text;
    MyText* _textLabel;
    QColor _backgroundColor;
    QPropertyAnimation *_animation;
};

class CustomMenu : public QWidget {
    Q_OBJECT

public:
    CustomMenu(QWidget* parent = nullptr) : QWidget(parent) {
        setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);

        _layout = new QVBoxLayout(this);
        _layout->setContentsMargins(0, 0, 0, 0);
        _layout->setSpacing(0);

        hide();
    }

    void addAction(const QString &text, std::function<void()> &callback) {
        MenuItem* settingsItem = new MenuItem(text);
        connect(settingsItem, &MenuItem::clicked, [callback, this]() {
            closeMenu();
            callback();
        });
        _layout->addWidget(settingsItem);
    }

private slots:
    void closeMenu() {
        close();
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event)
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        QPainterPath roundedPath;
        roundedPath.addRoundedRect(rect(), 8, 8);

        QRegion clipRegion(roundedPath.toFillPolygon().toPolygon());
        painter.setClipRegion(clipRegion);

        QStyleOption opt;
        opt.initFrom(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

        //painter.fillRect(rect(), style::trayBg);
    }

    void showEvent(QShowEvent* event) override {
        installEventFilter(this);
        QWidget::showEvent(event);

        QRect desiredGeometry = QRect(QCursor::pos(), sizeHint());

        QList<QScreen*> screens = QGuiApplication::screens();
        if (screens.isEmpty()) return;
        QScreen* screen = QGuiApplication::screenAt(QCursor::pos());
        QRect availableGeometry = screen->availableGeometry();

        int x = desiredGeometry.x();
        int y = desiredGeometry.y();

        if (x + desiredGeometry.width() > availableGeometry.width()) {
            x = availableGeometry.width() - desiredGeometry.width();
        }

        if (y + desiredGeometry.height() > availableGeometry.height()) {
            y = availableGeometry.height() - desiredGeometry.height();
        }

        move(x, y);
    }

    void hideEvent(QHideEvent* event) override {
        removeEventFilter(this);
        QWidget::hideEvent(event);
    }

    /*bool event(QEvent* event) override {
        if (event->type() == QEvent::FocusOut) {
            closeMenu();
        }
        return QWidget::event(event);
    }

    bool eventFilter(QObject* obj, QEvent* event) override {
        qDebug() << "event" << event->type();
        if (event->type() == QEvent::KeyPress) {
            qDebug() << "event" << "MEGA";
        }
        if (event->type() == QEvent::FocusOut) {
            closeMenu();
        }
        if (event->type() == QEvent::MouseButtonPress ||
            event->type() == QEvent::MouseMove ||
            event->type() == QEvent::MouseButtonRelease ||
            event->type() == QEvent::MouseButtonDblClick) {

            if (isVisible() && !rect().contains(mapFromGlobal(QCursor::pos()))) {
                closeMenu();
            }
        }
        return QWidget::eventFilter(obj, event);
    }*/

private:
    QVBoxLayout *_layout;
};

namespace Platform {

    struct Tray::Impl {
        QScopedPointer<QSystemTrayIcon> _icon;
        QScopedPointer<CustomMenu> _menu;

        void renderAndSetIcon(bool dark) const {
            const auto size = GetSystemMetrics(SM_CXSMICON); // icon size by x
            QColor color = dark ? QColorConstants::White : QColorConstants::Black;
            QIcon icon = renderIcon(size, color);
            _icon->setIcon(icon);
        }

        void themeChanged(Qt::ColorScheme colorScheme) const {
            /*std::optional<bool> darkMode;
            if (colorScheme != Qt::ColorScheme::Unknown) {
                darkMode = (colorScheme == Qt::ColorScheme::Dark);
            }
            renderAndSetIcon(darkMode.has_value() && darkMode.value());*/
            renderAndSetIcon(colorScheme == Qt::ColorScheme::Dark);
        }
    };

    Tray::Tray() : pImpl(new Impl) {
        pImpl->_icon.reset(new QSystemTrayIcon);
        pImpl->_icon->setToolTip(QApplication::applicationName());
        pImpl->_menu.reset(new CustomMenu);

        QObject::connect(pImpl->_icon.get(), &QSystemTrayIcon::activated, [this](){
            if (pImpl->_menu->isVisible()) {
                pImpl->_menu->close();
            } else {
                //pImpl->_menu->update();
                pImpl->_menu->show();
            }
        });

        updateIcon();

        // TODO: not working
        QObject::connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged,
                         [this](Qt::ColorScheme colorScheme) {
                             pImpl->themeChanged(colorScheme);
                         });

        pImpl->_icon->show();
    }

    Tray::~Tray() = default;

    void Tray::updateIcon() {
        pImpl->themeChanged(QGuiApplication::styleHints()->colorScheme());
    }

    void Tray::addAction(const QString &text, Fn<void()> &&callback) {
        pImpl->_menu->addAction(text, callback);
    }

} // namespace Platform

#include "tray_win.moc"