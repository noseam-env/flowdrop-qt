/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */

#include "settings_window.h"

#include <QApplication>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPainter>
#include <iostream>
#include <QLabel>
#include <QWidget>
#include <QDesktopServices>
#include <QPainterPath>
#include <QFileDialog>
#include <QPropertyAnimation>
#include "style.h"
#include "application.h"
#include "ui_util.h"

SettingsWindow *SettingsWindow::_instance = nullptr;


class VisualLine : public QFrame {
public:
    explicit VisualLine(QWidget *parent = nullptr) : QFrame(parent) {
        setFrameShape(QFrame::NoFrame);
        setFixedHeight(3);
        setMinimumWidth(30);

        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setSizePolicy(sizePolicy);
    }

    void paintEvent(QPaintEvent *event) override {
        QFrame::paintEvent(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        QPainterPath path;
        path.addRoundedRect(QRectF(0, 0, width(), height()), 1.5, 1.5);
        painter.fillPath(path, style::line);
    }
};

class DesignedRoundedButton : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)

public:
    explicit DesignedRoundedButton(QWidget *parent = nullptr) : QWidget(parent) {
        setFixedHeight(36);
        setCursor(Qt::PointingHandCursor);
        _hovered = false;

        _backgroundColor = style::button;
        _animation = new QPropertyAnimation(this, "backgroundColor", this);
        _animation->setDuration(100);

        _text = "Button";
        _font = QFont("Roboto", 11, QFont::Normal, false);

        _padding = 24;

        setMouseTracking(true);

        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        setSizePolicy(sizePolicy);
    }

    [[nodiscard]] QColor backgroundColor() const {
        return _backgroundColor;
    }

    void setBackgroundColor(const QColor &color) {
        _backgroundColor = color;
        update();
    }

    void setText(const QString &text) {
        _text = text;
        update();
    }

    [[nodiscard]] QSize sizeHint() const override {
        QFontMetrics fm(_font);
        QSize textSize = fm.size(Qt::TextSingleLine, _text);
        return {textSize.width() + 2 * _padding, 36};
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event)
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        QPainterPath path;
        path.addRoundedRect(rect(), 18, 18);

        QBrush bgColor(_backgroundColor);
        painter.fillPath(path, bgColor);

        QPen textPen(Qt::white);
        painter.setPen(textPen);
        painter.setFont(_font);

        QRectF textRect = rect().adjusted(_padding, 0, -_padding, 0);
        painter.drawText(textRect, Qt::AlignCenter, _text);
    }

    void resizeEvent(QResizeEvent *event) override {
        Q_UNUSED(event)
        if (_animation->state() == QPropertyAnimation::Running) {
            _animation->stop();
        }
        _animation->setStartValue(_backgroundColor);
        _animation->setEndValue(_hovered ? style::buttonHovered : style::button);
        _animation->start();
    }

    void mousePressEvent(QMouseEvent *event) override {
        Q_UNUSED(event)
        _animation->stop();
        _animation->setStartValue(_backgroundColor);
        _animation->setEndValue(style::buttonPressed);
        _animation->start();

        update();
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        Q_UNUSED(event)
        _animation->stop();
        _animation->setStartValue(_backgroundColor);
        _animation->setEndValue(_hovered ? style::buttonHovered : style::button);
        _animation->start();

        emit clicked();
    }

    void enterEvent(QEnterEvent *event) override {
        Q_UNUSED(event)
        _hovered = true;
        _animation->setStartValue(_backgroundColor);
        _animation->setEndValue(style::buttonHovered);
        _animation->start();
    }

    void leaveEvent(QEvent *event) override {
        Q_UNUSED(event)
        _hovered = false;
        _animation->setStartValue(_backgroundColor);
        _animation->setEndValue(style::button);
        _animation->start();
    }

signals:
    void clicked();

private:
    bool _hovered;
    int _padding;
    QString _text;
    QFont _font;
    QColor _backgroundColor;
    QPropertyAnimation *_animation;
};

class DesignedToggle : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(int ballX READ ballX WRITE setBallX)

public:
    explicit DesignedToggle(QWidget *parent = nullptr) : QWidget(parent) {
        setFixedSize(56, 32);
        setCursor(Qt::PointingHandCursor);

        _isToggled = false;
        _backgroundColor = style::toggleBg;
        _ballX = 4;
        _bgAnimation = new QPropertyAnimation(this, "backgroundColor", this);
        _bgAnimation->setDuration(100);
        _ballAnimation = new QPropertyAnimation(this, "ballX", this);
        _ballAnimation->setDuration(100);
    }

    [[nodiscard]] QColor backgroundColor() const {
        return _backgroundColor;
    }

    void setBackgroundColor(const QColor &color) {
        _backgroundColor = color;
        update();
    }

    [[nodiscard]] int ballX() const {
        return _ballX;
    }

    void setBallX(int x) {
        _ballX = x;
        update();
    }

    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);

        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(Qt::NoPen);
        painter.setBrush(_backgroundColor);
        painter.drawRoundedRect(rect(), 16, 16);

        painter.setBrush(style::toggleBall);
        painter.drawEllipse(QRect(_ballX, 4, 24, 24));
    }

    void mousePressEvent(QMouseEvent *) override {
        if (_ballAnimation->state() == QAbstractAnimation::Running) {
            _ballAnimation->stop();
        }

        _isToggled = !_isToggled;
        emit toggled(_isToggled);
        int endValue = _isToggled ? width() - 24 - 4 : 4;

        _ballAnimation->setStartValue(_ballX);
        _ballAnimation->setEndValue(endValue);
        _ballAnimation->start();
    }

    void enterEvent(QEnterEvent *event) override {
        Q_UNUSED(event)
        _bgAnimation->setStartValue(_backgroundColor);
        _bgAnimation->setEndValue(style::toggleBgHovered);
        _bgAnimation->start();
    }

    void leaveEvent(QEvent *event) override {
        Q_UNUSED(event)
        _bgAnimation->setStartValue(_backgroundColor);
        _bgAnimation->setEndValue(style::toggleBg);
        _bgAnimation->start();
    }

signals:
    void toggled(bool value);

private:
    bool _isToggled;
    QColor _backgroundColor;
    int _ballX;
    QPropertyAnimation* _bgAnimation;
    QPropertyAnimation* _ballAnimation;
};

class IconButton : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int rippleOpacity READ rippleOpacity WRITE setRippleOpacity)
    Q_PROPERTY(int rippleSize READ rippleSize WRITE setRippleSize)

public:
    explicit IconButton(QWidget *parent = nullptr) : QWidget(parent) {
        setFixedSize(36, 36);
        setCursor(Qt::PointingHandCursor);
        animation1 = new QPropertyAnimation(this, "rippleSize", this);
        animation1->setDuration(150);
        animation2 = new QPropertyAnimation(this, "rippleOpacity", this);
        animation2->setDuration(100);
        _rippleOpacity = 255;
        _rippleSize = 0;
    }

    [[nodiscard]] int rippleOpacity() const {
        return _rippleOpacity;
    }

    void setRippleOpacity(int opacity) {
        _rippleOpacity = opacity;
        update();
    }

    [[nodiscard]] int rippleSize() const {
        return _rippleSize;
    }

    void setRippleSize(int size) {
        _rippleSize = size;
        update();
    }

    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event)
        QPainter painter(this);

        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(Qt::NoPen);

        painter.setBrush(style::iconBtnBg);
        painter.drawEllipse(rect());

        if (_rippleSize > 0) {
            auto rippleColor = QColor(style::iconBtnRipple);
            rippleColor.setAlpha(_rippleOpacity);
            painter.setBrush(rippleColor);
            int rippleOffset = (width() - _rippleSize) / 2;
            painter.drawEllipse(QRect(rippleOffset, rippleOffset, _rippleSize, _rippleSize));
        }

        int iconSize = 20;

        QImage image(iconSize, iconSize, QImage::Format_ARGB32);
        image.fill(Qt::transparent);

        QPainter svgPainter(&image);

        QSvgRenderer renderer(Resources::icons::coffee);
        renderer.render(&svgPainter);

        svgPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        svgPainter.fillRect(image.rect(), style::iconBtn);

        painter.drawImage(QPoint((width() - iconSize) / 2, (height() - iconSize) / 2), image);
    }

    void mousePressEvent(QMouseEvent *event) override {
        Q_UNUSED(event)

        if (animation1->state() == QAbstractAnimation::Running) {
            animation1->stop();
        }
        animation1->setStartValue(0);
        animation1->setEndValue(width());
        animation1->start();
        _rippleOpacity = 255;
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        Q_UNUSED(event)
        emit clicked();

        if (animation2->state() == QAbstractAnimation::Running) {
            animation2->stop();
        }
        animation2->setStartValue(255);
        animation2->setEndValue(0);
        animation2->start();
    }

signals:
    void clicked();

private:
    int _rippleOpacity;
    int _rippleSize;
    QPropertyAnimation* animation1;
    QPropertyAnimation* animation2;
};

class ClickableText : public MyText {
    Q_OBJECT

public:
    explicit ClickableText(const QString &text, int pointSize = 13, QWidget *parent = nullptr) : MyText(text, pointSize, parent) {
        setCursor(Qt::PointingHandCursor);
    }

signals:
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override {
        QLabel::mouseReleaseEvent(event);
        emit clicked();
    }

    void enterEvent(QEnterEvent *event) override {
        Q_UNUSED(event)
        QFont newFont = QFont(font());
        newFont.setUnderline(true);
        setFont(newFont);
    }

    void leaveEvent(QEvent *event) override {
        Q_UNUSED(event)
        QFont newFont = QFont(font());
        newFont.setUnderline(false);
        setFont(newFont);
    }
};

class SettingElement : public QWidget {
public:
    explicit SettingElement(QWidget *parent = nullptr) : QWidget(parent) {
        auto *layout = new QHBoxLayout(this);
        layout->setContentsMargins(10, 6, 10, 6);
        layout->setSpacing(20);

        auto *leftWidget = new QWidget(this);
        layout->addWidget(leftWidget);

        _leftLayout = new QVBoxLayout(leftWidget);

        auto* visualLine = new VisualLine(this);
        layout->addWidget(visualLine);

        auto *rightWidget = new QWidget(this);
        layout->addWidget(rightWidget);

        _rightLayout = new QVBoxLayout(rightWidget);
    }

    void addLeftWidget(QWidget *widget) {
        _leftLayout->addWidget(widget);
    }

    void addRightWidget(QWidget *widget) {
        _rightLayout->addWidget(widget);
    }

private:
    QVBoxLayout *_leftLayout;
    QVBoxLayout *_rightLayout;
};

SettingsWindow::SettingsWindow() : QMainWindow() {
    setWindowTitle(QApplication::applicationName() + " Settings");
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);

    QPalette windowPalette;
    windowPalette.setColor(QPalette::Window, style::bg1);
    this->setAutoFillBackground(true);
    this->setPalette(windowPalette);

    auto *centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    auto *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    auto *widget1 = new QWidget(centralWidget);
    mainLayout->addWidget(widget1);

    QPalette palette1;
    palette1.setColor(QPalette::Window, style::bg2);
    widget1->setAutoFillBackground(true);
    widget1->setPalette(palette1);

    auto *settingsLayout = new QVBoxLayout(widget1);
    settingsLayout->setContentsMargins(20, 20, 20, 20);
    settingsLayout->setSpacing(4);

    auto *element1 = new SettingElement(widget1);
    settingsLayout->addWidget(element1);

    auto *label1 = new MyText("Send files", 15);
    label1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label1->setColor(style::text1);
    element1->addLeftWidget(label1);

    auto *btn1 = new DesignedRoundedButton(element1);
    btn1->setText("Select files and send");
    element1->addRightWidget(btn1);

    QObject::connect(btn1, &DesignedRoundedButton::clicked, []() {
        App().selectFilesAndSend();
    });

    auto *element2 = new SettingElement(widget1);
    settingsLayout->addWidget(element2);


    auto *label2_1 = new MyText("Received files will be saved to", 15);
    label2_1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label2_1->setColor(style::text1);
    element2->addLeftWidget(label2_1);

    auto *label2_2 = new MyText(App().getDestDir(), 11);
    label2_2->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label2_2->setColor(style::text2);
    element2->addLeftWidget(label2_2);

    auto *btn2 = new DesignedRoundedButton(element2);
    btn2->setText("Change");
    element2->addRightWidget(btn2);

    QObject::connect(btn2, &DesignedRoundedButton::clicked, [label2_2]() {
        QString folderPath = QFileDialog::getExistingDirectory(nullptr, "Select Directory", App().getDestDir());
        if (folderPath.isEmpty()) return;
        App().setDestDir(folderPath);
        label2_2->setText(folderPath);
    });

    auto *element3 = new SettingElement(widget1);
    settingsLayout->addWidget(element3);

    auto *label3 = new MyText("Auto accept requests", 15);
    label3->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label3->setColor(style::text1);
    element3->addLeftWidget(label3);

    auto *toggle3 = new DesignedToggle(element3);
    element3->addRightWidget(toggle3);

    QObject::connect(toggle3, &DesignedToggle::toggled, [](bool value) {

    });

    /*auto *element4 = new SettingElement(widget1);
    settingsLayout->addWidget(element4);

    auto *label4 = new MyText("Run on startup", 15);
    label4->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label4->setColor(style::text1);
    element4->addLeftWidget(label4);

    auto *toggle4 = new DesignedToggle(element4);
    element4->addRightWidget(toggle4);

    QObject::connect(toggle4, &DesignedToggle::toggled, [](bool value) {

    });*/

    // Footer

    auto *widget2 = new QWidget(centralWidget);
    widget2->setFixedHeight(80);
    mainLayout->addWidget(widget2);

    auto *footerLayout = new QHBoxLayout(widget2);
    footerLayout->setContentsMargins(22, 0, 16, 0);
    footerLayout->setSpacing(22);

    /*auto *leftWrapper = new QWidget(widget2);
    leftWrapper->setFixedSize(60, 60);
    footerLayout->addWidget(leftWrapper);

    auto* leftWrapperLayout = new QVBoxLayout(leftWrapper);
    leftWrapperLayout->setContentsMargins(0, 0, 0, 0);
    leftWrapperLayout->setSpacing(0);*/

    auto *donateBtn = new IconButton(widget2);
    footerLayout->addWidget(donateBtn);
    QObject::connect(donateBtn, &IconButton::clicked, []() {
        QUrl url("https://flowdrop.site/donate");
        QDesktopServices::openUrl(url);
    });
    //leftWrapperLayout->addWidget(donateBtn);

    auto *footerTextLayout = new QVBoxLayout();
    footerTextLayout->setContentsMargins(0, 0, 0, 0);
    footerTextLayout->setSpacing(0);
    footerLayout->addLayout(footerTextLayout);

    footerTextLayout->addStretch(1);

    auto *footerText1 = new MyText("FlowDrop Qt 0.0.1", 11);
    footerText1->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    footerText1->setColor(style::text4);
    footerTextLayout->addWidget(footerText1);

    auto *footerText2LineLayout = new QHBoxLayout();
    footerText2LineLayout->setContentsMargins(0, 0, 0, 0);
    footerText2LineLayout->setSpacing(0);
    footerTextLayout->addLayout(footerText2LineLayout);

    auto *footerText2 = new MyText("This software is licensed under ", 11);
    footerText2->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    footerText2->setColor(style::text4);
    footerText2LineLayout->addWidget(footerText2);

    auto *footerText3 = new ClickableText("GNU GPL", 11);
    footerText3->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    footerText3->setColor(style::text3);
    footerText2LineLayout->addWidget(footerText3);
    QObject::connect(footerText3, &ClickableText::clicked, []() {
        QUrl url("https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL");
        QDesktopServices::openUrl(url);
    });

    auto *footerText4 = new MyText(" version 3 with anti-commercial clause.", 11);
    footerText4->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    footerText4->setColor(style::text4);
    footerText2LineLayout->addWidget(footerText4);

    auto *footerText3LineLayout = new QHBoxLayout();
    footerText3LineLayout->setContentsMargins(0, 0, 0, 0);
    footerText3LineLayout->setSpacing(0);
    footerTextLayout->addLayout(footerText3LineLayout);

    auto *footerText5 = new MyText("Source code is available on ", 11);
    footerText5->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    footerText5->setColor(style::text4);
    footerText3LineLayout->addWidget(footerText5);

    auto *footerText6 = new ClickableText("GitHub", 11);
    footerText6->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    footerText6->setColor(style::text3);
    footerText3LineLayout->addWidget(footerText6);
    QObject::connect(footerText6, &ClickableText::clicked, []() {
        QUrl url("https://github.com/noseam-env/flowdrop-qt");
        QDesktopServices::openUrl(url);
    });

    auto *footerText7 = new MyText(".", 11);
    footerText7->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    footerText7->setColor(style::text4);
    footerText3LineLayout->addWidget(footerText7);

    footerText3LineLayout->addStretch(1);

    footerTextLayout->addStretch(1);

    setFixedSize(sizeHint());
}

void SettingsWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized()) {
            hide();
        }
    }
    QMainWindow::changeEvent(event);
}

#include "settings_window.moc"