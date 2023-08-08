/*
 * This file is part of FlowDrop Qt.
 *
 * For license and copyright information please follow this link:
 * https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL
 */

#include "receivers_window.h"

#include <QThread>
#include <QApplication>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QListView>
#include <QStringListModel>
#include <QPainterPath>
#include <QScrollArea>
#include <QPushButton>
#include "flowdrop/flowdrop.hpp"
#include "application.h"
#include "style.h"
#include "ui_util.h"
#include "qtmaterialcircularprogress.h"

class DesignedScrollBar : public QScrollBar {
    Q_OBJECT

public:
    explicit DesignedScrollBar(QWidget* parent = nullptr) : QScrollBar(Qt::Vertical, parent) {
        setMaximumWidth(6);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }

    [[nodiscard]] QSize sizeHint() const override {
        return {380, 100};
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event)

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        painter.setBrush(style::scrollbar);

        int x, y, w, h;
        rect().getRect(&x, &y, &w, &h);

        const qreal q = (Qt::Horizontal == orientation() ? w : h) /
                        static_cast<qreal>(maximum()-minimum()+pageStep()-1);

        QRect handle = Qt::Horizontal == orientation()
                       ? QRect(sliderPosition() * q, y, pageStep() * q, h)
                       : QRect(x, sliderPosition() * q, w, pageStep() * q);

        painter.drawRoundedRect(handle, 3, 3);
    }
};

class SVGIcon : public QWidget {
public:
    explicit SVGIcon(QWidget* parent = nullptr)
            : QWidget(parent) {
    }

    bool loadSvg(const QString& filePath) {
        _svgRenderer = new QSvgRenderer(filePath, this);
        if (_svgRenderer->isValid()) {
            QSize svgSize = _svgRenderer->defaultSize();
            setFixedSize(svgSize);
            return true;
        } else {
            delete _svgRenderer;
            _svgRenderer = nullptr;
            return false;
        }
    }

    void setColor(QColor color) {
        _color = color;
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event)
        if (_svgRenderer) {
            QPainter painter(this);
            painter.setRenderHint(QPainter::Antialiasing);

            QImage image(size(), QImage::Format_ARGB32);
            image.fill(Qt::transparent);
            QPainter imagePainter(&image);

            _svgRenderer->render(&imagePainter);

            imagePainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            imagePainter.fillRect(image.rect(), _color);
            imagePainter.setCompositionMode(QPainter::CompositionMode_DestinationOver);

            painter.drawImage(0, 0, image);
        }
    }

private:
    QSvgRenderer* _svgRenderer = nullptr;
    QColor _color;
};

bool equalsIgnoreCase(const QString &left, const QString &right) {
    return left.compare(right, Qt::CaseInsensitive) == 0;
}

class Receiver : public QWidget {
    Q_OBJECT

public:
    explicit Receiver(const flowdrop::DeviceInfo &deviceInfo, QWidget* parent = nullptr) : QWidget(parent), _deviceInfo(deviceInfo) {
        setFixedHeight(60);

        auto *layout = new QHBoxLayout(this);
        layout->setContentsMargins(30, 0, 24, 0);
        layout->setSpacing(20);

        QString icon = Resources::icons::device_pc;
        if (_deviceInfo.platform.has_value()) {
            QString platform = QString(_deviceInfo.platform.value().c_str());
            if (equalsIgnoreCase(platform, "android")) {
                icon = Resources::icons::device_android;
            } else if (equalsIgnoreCase(platform, "ios")) {
                icon = Resources::icons::device_ios;
            } else if (equalsIgnoreCase(platform, "macos")) {
                icon = Resources::icons::device_mac;
            }
        }

        auto *imageWidget = new SVGIcon();
        imageWidget->loadSvg(icon);
        imageWidget->setColor(style::text1);
        layout->addWidget(imageWidget);

        auto *textLayout = new QVBoxLayout();
        textLayout->setContentsMargins(0, 0, 0, 0);
        textLayout->setSpacing(0);
        textLayout->addStretch(1);
        layout->addLayout(textLayout);

        std::string name = _deviceInfo.name.value_or(_deviceInfo.model.value_or(_deviceInfo.id));

        auto *nameText = new MyText(QString(name.c_str()), 15);
        nameText->setColor(style::text1);
        textLayout->addWidget(nameText);

        if (_deviceInfo.platform.has_value()) {
            std::string system = _deviceInfo.platform.value();
            if (_deviceInfo.system_version.has_value()) {
                system += " " + _deviceInfo.system_version.value();
            }

            auto *systemText = new MyText(QString(system.c_str()), 11);
            systemText->setColor(style::text2);
            textLayout->addWidget(systemText);
        }

        textLayout->addStretch(1);

        layout->addStretch(1);

        qInfo() << "Added receiver:" << _deviceInfo.id;

        setWidgetBackgroundColor(this, style::bg2Hovered);
        setAutoFillBackground(false);

        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }

    /*[[nodiscard]] QSize sizeHint() const override {
        return {374, 50};
    }*/

signals:
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override {
        QWidget::mouseReleaseEvent(event);
        emit clicked();
    }

    void enterEvent(QEnterEvent *event) override {
        Q_UNUSED(event)
        setAutoFillBackground(true);
    }

    void leaveEvent(QEvent *event) override {
        Q_UNUSED(event)
        setAutoFillBackground(false);
    }

private:
    const flowdrop::DeviceInfo &_deviceInfo;
};

class ScrollAreaContent : public QWidget {
public:
    explicit ScrollAreaContent(QWidget *parent = nullptr) : QWidget(parent) {
        _contentLayout = new QVBoxLayout(this);
        _contentLayout->setContentsMargins(0, 0, 0, 0);
        _contentLayout->setSpacing(0);
        _contentLayout->setAlignment(Qt::AlignTop);
        setLayout(_contentLayout);
        // FIXME: bug with scroll area still here...
        setMinimumSize(200, 200);
    }

public:
    QVBoxLayout *_contentLayout;
};

ReceiversWindow::ReceiversWindow(const QStringList& fileNames) : _fileNames(fileNames) {
    setWindowTitle(QApplication::applicationName());
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setFixedSize(380, 400);

    setWidgetBackgroundColor(this, style::bg1);

    auto *centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    auto *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // header

    auto *widget1 = new QWidget(centralWidget);
    mainLayout->addWidget(widget1);

    auto *headerLayout = new QHBoxLayout(widget1);
    headerLayout->setContentsMargins(20, 20, 20, 20);
    headerLayout->setSpacing(20);

    auto *headerTextLayout = new QVBoxLayout();
    headerTextLayout->setContentsMargins(0, 0, 0, 0);
    headerTextLayout->setSpacing(8);
    headerLayout->addLayout(headerTextLayout);

    headerTextLayout->addStretch(1);

    auto *headerText1 = new MyText("Select receivers", 15);
    headerText1->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    headerText1->setColor(style::text1);
    headerTextLayout->addWidget(headerText1);

    auto *headerText2 = new MyText("Make sure that the devices you want to\nsend files to are on your local network.", 11);
    headerText2->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    headerText2->setColor(style::text2);
    headerTextLayout->addWidget(headerText2);

    headerTextLayout->addStretch(1);


    auto *circularProgress = new QtMaterialCircularProgress();
    circularProgress->setLineWidth(5);
    circularProgress->setSize(40);
    circularProgress->setColor(style::text1);
    headerLayout->addWidget(circularProgress);

    // content

    /*auto *widget2 = new QWidget(centralWidget);
    setWidgetBackgroundColor(widget2, style::bg2);
    mainLayout->addWidget(widget2, 1);

    auto *widget2layout = new QVBoxLayout(widget2);
    widget2layout->setContentsMargins(0, 0, 0, 0);*/

    auto *scrollContent = new ScrollAreaContent();
    setWidgetBackgroundColor(scrollContent, style::bg2);
    _contentLayout = scrollContent->_contentLayout;

    auto *scrollArea = new QScrollArea();
    scrollArea->setContentsMargins(0, 0, 0, 0);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(scrollContent);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBar(new DesignedScrollBar);

    //widget2layout->addWidget(scrollArea, 1);
    mainLayout->addWidget(scrollArea, 1);

    // this needed for testing scroll area bugs

    /*auto *receiver1 = new Receiver(App().deviceInfo());
    _contentLayout->addWidget(receiver1);
    auto *receiver2 = new Receiver(App().deviceInfo());
    _contentLayout->addWidget(receiver2);
    auto *receiver3 = new Receiver(App().deviceInfo());
    _contentLayout->addWidget(receiver3);
    auto *receiver4 = new Receiver(App().deviceInfo());
    _contentLayout->addWidget(receiver4);
    auto *receiver5 = new Receiver(App().deviceInfo());
    _contentLayout->addWidget(receiver5);*/

    // logic

    _discoverThread = new QThread;
    QObject::connect(_discoverThread, &QThread::started, [this](){
        flowdrop::setDebug(true);
        flowdrop::discover([this](const flowdrop::DeviceInfo &deviceInfo){
            if (deviceInfo.id == App().deviceInfo().id) return;
            QMetaObject::invokeMethod(this, "addReceiver", Qt::QueuedConnection, Q_ARG(flowdrop::DeviceInfo, deviceInfo));
        }, [this](){
            return _stopDiscover.load();
        });
    });
    _discoverThread->start();

    qDebug() << fileNames;
}

void ReceiversWindow::addReceiver(const flowdrop::DeviceInfo &deviceInfo) {
    auto *receiver = new Receiver(deviceInfo);
    _contentLayout->addWidget(receiver);
    QObject::connect(receiver, &Receiver::clicked, [this, deviceInfo](){
        auto *thread = new QThread;
        auto *fileNames = new QStringList(_fileNames);
        QObject::connect(thread, &QThread::started, [deviceInfo, fileNames](){
            App().sendTo(QString(deviceInfo.id.c_str()), *fileNames);
        });
        thread->start();
        close();
    });
}

void ReceiversWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        if (isHidden()) {
            _stopDiscover = true;
            _discoverThread->quit();
            _discoverThread->wait();
            delete _discoverThread;
            _discoverThread = nullptr;
        }
    }
    QMainWindow::changeEvent(event);
}

#include "receivers_window.moc"