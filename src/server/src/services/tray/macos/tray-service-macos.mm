#include "services/tray/macos/tray-service-macos.hpp"
#import <AppKit/AppKit.h>
#include <QImage>
#include <QPainter>
#include <QSvgRenderer>
#include <QString>

namespace {
constexpr qreal MENU_BAR_ICON_POINT_SIZE = 18.0;
constexpr qreal MENU_BAR_ICON_SCALE = 2.0;

NSImage *renderTrayImage() {
  const int pixelSize = static_cast<int>(MENU_BAR_ICON_POINT_SIZE * MENU_BAR_ICON_SCALE);
  QSvgRenderer renderer(QStringLiteral(":icons/vicinae.svg"));
  QImage image(QSize(pixelSize, pixelSize), QImage::Format_RGBA8888_Premultiplied);
  image.fill(Qt::transparent);
  {
    QPainter painter(&image);
    renderer.render(&painter);
  }

  NSData *data = [NSData dataWithBytes:image.constBits() length:image.sizeInBytes()];
  CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
  CGDataProviderRef provider = CGDataProviderCreateWithCFData((__bridge CFDataRef)data);
  CGImageRef cgImage =
      CGImageCreate(image.width(), image.height(), 8, 32, image.bytesPerLine(), colorSpace,
                    static_cast<CGBitmapInfo>(kCGImageAlphaPremultipliedLast | kCGBitmapByteOrderDefault),
                    provider, nullptr, false, kCGRenderingIntentDefault);

  NSImage *nsImage = [[NSImage alloc]
      initWithCGImage:cgImage
                 size:NSMakeSize(MENU_BAR_ICON_POINT_SIZE, MENU_BAR_ICON_POINT_SIZE)];
  [nsImage setTemplate:YES];

  CGImageRelease(cgImage);
  CGDataProviderRelease(provider);
  CGColorSpaceRelease(colorSpace);
  return nsImage;
}
} // namespace

@interface VicinaeTrayTarget : NSObject
@property(nonatomic, assign) TrayServiceMacOS *owner;
@end

@implementation VicinaeTrayTarget
- (void)toggle:(id)sender {
  if (self.owner) self.owner->emitToggle();
}
- (void)openAbout:(id)sender {
  if (self.owner) self.owner->emitOpenSettings(QStringLiteral("about"));
}
- (void)openPreferences:(id)sender {
  if (self.owner) self.owner->emitOpenSettings(QString());
}
- (void)quit:(id)sender {
  if (self.owner) self.owner->emitQuit();
}
@end

struct TrayServiceMacOS::Impl {
  NSStatusItem *statusItem = nil;
  NSMenuItem *versionItem = nil;
  VicinaeTrayTarget *target = nil;
};

TrayServiceMacOS::TrayServiceMacOS(QObject *parent) : TrayService(parent), m_impl(std::make_unique<Impl>()) {
  m_impl->target = [[VicinaeTrayTarget alloc] init];
  m_impl->target.owner = this;

  m_impl->statusItem = [[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength];
  m_impl->statusItem.button.image = renderTrayImage();
  m_impl->statusItem.button.toolTip = @"Vicinae";

  NSMenu *menu = [[NSMenu alloc] init];

  NSMenuItem *toggleItem = [[NSMenuItem alloc] initWithTitle:@"Toggle Vicinae"
                                                      action:@selector(toggle:)
                                               keyEquivalent:@""];
  toggleItem.target = m_impl->target;
  [menu addItem:toggleItem];

  m_impl->versionItem = [[NSMenuItem alloc] initWithTitle:@"Vicinae" action:nil keyEquivalent:@""];
  [menu addItem:m_impl->versionItem];

  [menu addItem:[NSMenuItem separatorItem]];

  NSMenuItem *aboutItem = [[NSMenuItem alloc] initWithTitle:@"About Vicinae"
                                                     action:@selector(openAbout:)
                                              keyEquivalent:@""];
  aboutItem.target = m_impl->target;
  [menu addItem:aboutItem];

  NSMenuItem *prefsItem = [[NSMenuItem alloc] initWithTitle:@"Preferences…"
                                                     action:@selector(openPreferences:)
                                              keyEquivalent:@","];
  prefsItem.keyEquivalentModifierMask = NSEventModifierFlagCommand;
  prefsItem.target = m_impl->target;
  [menu addItem:prefsItem];

  [menu addItem:[NSMenuItem separatorItem]];

  NSMenuItem *quitItem = [[NSMenuItem alloc] initWithTitle:@"Quit Vicinae"
                                                    action:@selector(quit:)
                                             keyEquivalent:@""];
  quitItem.target = m_impl->target;
  [menu addItem:quitItem];

  m_impl->statusItem.menu = menu;
}

TrayServiceMacOS::~TrayServiceMacOS() {
  if (m_impl->statusItem) { [[NSStatusBar systemStatusBar] removeStatusItem:m_impl->statusItem]; }
}

void TrayServiceMacOS::setVersion(const QString &version) {
  if (!m_impl->versionItem) return;
  m_impl->versionItem.title = [NSString stringWithFormat:@"Vicinae %@", version.toNSString()];
}

void TrayServiceMacOS::show() { m_impl->statusItem.visible = YES; }

void TrayServiceMacOS::hide() { m_impl->statusItem.visible = NO; }

void TrayServiceMacOS::emitToggle() { emit toggleRequested(); }

void TrayServiceMacOS::emitOpenSettings(const QString &tab) { emit openSettingsRequested(tab); }

void TrayServiceMacOS::emitQuit() { emit quitRequested(); }
