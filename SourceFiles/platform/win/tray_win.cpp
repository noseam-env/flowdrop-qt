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


class TrayMenuItem : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)

public:
    TrayMenuItem(const QString& text, QWidget* parent = nullptr) : QWidget(parent), _text(text) {
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

    void reset() {
        _animation->stop();
        setBackgroundColor(style::trayBg);
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event)
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        painter.setBrush(_backgroundColor);
        painter.drawRoundedRect(rect(), 8, 8);
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

class TrayMenu : public QWidget {
    Q_OBJECT

public:
    explicit TrayMenu(QWidget* parent = nullptr) : QWidget(parent) {
        setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);
        setMouseTracking(true);
        setFocusPolicy(Qt::WheelFocus);

        _layout = new QVBoxLayout(this);
        _layout->setContentsMargins(0, 0, 0, 0);
        _layout->setSpacing(0);

        hide();
    }

    void addAction(const QString &text, std::function<void()> &callback) {
        auto* settingsItem = new TrayMenuItem(text);
        connect(settingsItem, &TrayMenuItem::clicked, [callback, this]() {
            close();
            callback();
        });
        _layout->addWidget(settingsItem);
    }

    bool prepareGeometryFor(const QPoint &p) {
        QList<QScreen*> screens = QGuiApplication::screens();
        if (screens.isEmpty()) return false;

        QScreen* screen = QGuiApplication::screenAt(p);
        this->setScreen(screen);

        QRect availableGeometry = screen->availableGeometry();

        QSize windowSize = this->sizeHint();
        int width = windowSize.width();
        int height = windowSize.height();

        auto result = p;

        // offset this little bit:
        int offset = 16;
        if (result.x() - offset > availableGeometry.x() - offset) {
            result.setX(result.x() - offset);
        }

        if (result.x() + width > availableGeometry.x() + availableGeometry.width()) {
            result.setX(availableGeometry.x() + availableGeometry.width() - width);
        }
        if (result.x() < availableGeometry.x()) {
            result.setX(availableGeometry.x());
        }
        if (result.y() + height > availableGeometry.y() + availableGeometry.height()) {
            result.setY(availableGeometry.y() + availableGeometry.height() - height);
        }
        if (result.y() < availableGeometry.y()) {
            result.setY(availableGeometry.y());
        }

        this->move(result);
        return true;
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        QWidget::paintEvent(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.setPen(Qt::NoPen);
        painter.setBrush(style::trayBg);
        painter.drawRoundedRect(rect(), 8, 8);
    }

    void showEvent(QShowEvent* event) override {
        QWidget::showEvent(event);
        prepareGeometryFor(QCursor::pos());
        // snail code sorry
        for (int i = 0; i < _layout->count(); ++i) {
            QLayoutItem *item = _layout->itemAt(i);
            if (item) {
                QWidget *widget = item->widget();
                if (widget) {
                    ((TrayMenuItem *) widget)->reset();
                }
            }
        }
    }

    void focusOutEvent(QFocusEvent *event) override {
        close();
    }

private:
    QVBoxLayout *_layout;
};

namespace Platform {

    struct Tray::Impl {
        QScopedPointer<QSystemTrayIcon> _icon;
        QScopedPointer<TrayMenu> _menu;

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
        pImpl->_menu.reset(new TrayMenu);

        QObject::connect(pImpl->_icon.get(), &QSystemTrayIcon::activated, [this](){
            if (pImpl->_menu->isVisible()) {
                pImpl->_menu->close();
            } else {
                //pImpl->_menu->update();
                pImpl->_menu->show();
                pImpl->_menu->activateWindow();
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