//
// Created by nelon on 7/9/2023.
//

#include "platform/platform_tray.h"
#include "application.h"
#include <QMenu>
#include <QObject>
#include <QScreen>
#include <QVBoxLayout>
#include <QStyleHints>
#include <QApplication>
#include <QStyleFactory>
#include <QScopedPointer>
#include <QSystemTrayIcon>
#include <memory>

#include <Cocoa/Cocoa.h>

#import <AppKit/NSMenu.h>
#import <AppKit/NSStatusBar.h>
#import <AppKit/NSStatusBarButton.h>
#import <AppKit/NSStatusItem.h>

inline NSString *Q2NSString(const QString &str) {
    return [NSString stringWithUTF8String:str.toUtf8().constData()];
}

inline NSImage *Q2NSImage(const QImage &image) {
    if (image.isNull()) {
        return nil;
    }
    CGImageRef cgImage = image.toCGImage();
    if (!cgImage) {
        return nil;
    }
    auto nsImage = [[NSImage alloc] initWithSize:NSZeroSize];
    auto *imageRep = [[NSBitmapImageRep alloc] initWithCGImage:cgImage];
    imageRep.size = (image.size() / image.devicePixelRatioF()).toCGSize();
    [nsImage addRepresentation:[imageRep autorelease]];
    CFRelease(cgImage);

    return [nsImage autorelease];
}

@interface CommonDelegate : NSObject<NSMenuDelegate> {}

- (void) menuDidClose:(NSMenu *)menu;
- (void) menuWillOpen:(NSMenu *)menu;
- (void) observeValueForKeyPath:(NSString *)keyPath
                       ofObject:(id)object
                         change:(NSDictionary<NSKeyValueChangeKey, id> *)change
                        context:(void *)context;

@end // @interface CommonDelegate

@implementation CommonDelegate {}

- (void) menuDidClose:(NSMenu *)menu {
    /*Core::Sandbox::Instance().customEnterFromEventLoop([&] {
        _closes.fire({});
    });*/
}

- (void) menuWillOpen:(NSMenu *)menu {
    /*Core::Sandbox::Instance().customEnterFromEventLoop([&] {
        _aboutToShowRequests.fire({});
    });*/
}

// Thanks https://stackoverflow.com/a/64525038
- (void) observeValueForKeyPath:(NSString *)keyPath
                       ofObject:(id)object
                         change:(NSDictionary<NSKeyValueChangeKey, id> *)change
                        context:(void *)context {
    if ([keyPath isEqualToString:@"button.effectiveAppearance"]) {
        //_appearanceChanges.fire({});
    }
}

@end // @implementation MenuDelegate

class NativeIcon final {
public:
    NativeIcon();
    ~NativeIcon();

    void setMenu(QMenu *menu); // TODO: not null

    void updateIcon();

private:
    CommonDelegate *_delegate;
    NSStatusItem *_status;
};

NativeIcon::NativeIcon() : _delegate([[CommonDelegate alloc] init])
, _status([[NSStatusBar.systemStatusBar statusItemWithLength:NSSquareStatusItemLength] retain]) {

    [_status
        addObserver:_delegate
        forKeyPath:@"button.effectiveAppearance"
        options:NSKeyValueObservingOptionNew | NSKeyValueObservingOptionInitial
        context:nil];

    _status.button.toolTip = Q2NSString(QApplication::applicationName());
}

NativeIcon::~NativeIcon() {
    [_status
        removeObserver:_delegate
        forKeyPath:@"button.effectiveAppearance"];
    [NSStatusBar.systemStatusBar removeStatusItem:_status];

    [_status release];
    [_delegate release];
}

void NativeIcon::setMenu(QMenu *menu) {
    _status.menu = menu->toNSMenu();
    _status.menu.delegate = _delegate;
    //[_status.button performClick:nil];
}

void NativeIcon::updateIcon() {
    const auto appearance = _status.button.effectiveAppearance;
    const auto darkMode = [[appearance.name lowercaseString] containsString:@"dark"];

    // The recommended maximum title bar icon height is 18 points
    // (device independent pixels). The menu height on past and
    // current OS X versions is 22 points. Provide some future-proofing
    // by deriving the icon height from the menu height.
    const int padding = 0;
    const int menuHeight = NSStatusBar.systemStatusBar.thickness;
    // [[status.button window] backingScaleFactor];
    const int maxImageHeight = (menuHeight - padding) * qApp->devicePixelRatio();

    // Select pixmap based on the device pixel height. Ideally we would use
    // the devicePixelRatio of the target screen, but that value is not
    // known until draw time. Use qApp->devicePixelRatio, which returns the
    // devicePixelRatio for the "best" screen on the system.

    const auto side = 22 * qApp->devicePixelRatio();

    QColor color = darkMode ? QColorConstants::White : QColorConstants::Black;

    QImage finalIcon(side, side, QImage::Format_ARGB32);
    finalIcon.fill(Qt::transparent);

    QPixmap icon = renderIcon(16, color);

    int x = (finalIcon.width() - icon.width()) / 2;
    int y = (finalIcon.height() - icon.height()) / 2;

    QPainter painter(&finalIcon);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawPixmap(x, y, icon);
    painter.end();

    _status.button.image = Q2NSImage(finalIcon);
    _status.button.alternateImage = Q2NSImage(finalIcon);
    _status.button.imageScaling = NSImageScaleProportionallyDown;
}

namespace Platform {

    struct Tray::Impl {
        std::unique_ptr<NativeIcon> _nativeIcon;
        QMenu *_menu;

        void updateIcon() {
            if (_nativeIcon) {
                _nativeIcon->updateIcon();
            }
        }
    };

    Tray::Tray() : pImpl(new Impl) {
        pImpl->_nativeIcon = std::make_unique<NativeIcon>();

        pImpl->_menu = new QMenu();

        pImpl->_nativeIcon->setMenu(pImpl->_menu);

        updateIcon();
    }

    Tray::~Tray() = default;

    void Tray::updateIcon() {
        pImpl->updateIcon();
    }

    void Tray::addAction(const QString &text, Fn<void()> &&callback) {
        pImpl->_menu->addAction(text, callback);
    }

} // namespace Platform
