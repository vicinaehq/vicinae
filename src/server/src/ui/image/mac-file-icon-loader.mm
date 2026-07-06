#include "mac-file-icon-loader.hpp"

#import <AppKit/AppKit.h>

namespace {

QImage renderNSImage(NSImage *image, const QSize &size) {
  if (!image || !size.isValid() || size.isEmpty()) return {};

  NSInteger const width = size.width();
  NSInteger const height = size.height();
  NSInteger const bytesPerRow = width * 4;

  NSBitmapImageRep *target =
      [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:NULL
                                              pixelsWide:width
                                              pixelsHigh:height
                                           bitsPerSample:8
                                         samplesPerPixel:4
                                                hasAlpha:YES
                                                isPlanar:NO
                                          colorSpaceName:NSCalibratedRGBColorSpace
                                             bytesPerRow:bytesPerRow
                                            bitsPerPixel:32];
  if (!target) return {};

  NSGraphicsContext *ctx = [NSGraphicsContext graphicsContextWithBitmapImageRep:target];
  if (!ctx) return {};

  NSSize const natural = image.size;
  if (natural.width <= 0 || natural.height <= 0) return {};

  CGFloat const scale = MIN(width / natural.width, height / natural.height);
  CGFloat const drawWidth = natural.width * scale;
  CGFloat const drawHeight = natural.height * scale;
  NSRect const drawRect =
      NSMakeRect((width - drawWidth) / 2.0, (height - drawHeight) / 2.0, drawWidth, drawHeight);

  [NSGraphicsContext saveGraphicsState];
  [NSGraphicsContext setCurrentContext:ctx];
  [ctx setImageInterpolation:NSImageInterpolationHigh];
  [[NSColor clearColor] setFill];
  NSRectFillUsingOperation(NSMakeRect(0, 0, width, height), NSCompositingOperationCopy);
  [image drawInRect:drawRect
           fromRect:NSZeroRect
          operation:NSCompositingOperationSourceOver
           fraction:1.0
     respectFlipped:YES
              hints:nil];
  [NSGraphicsContext restoreGraphicsState];

  QImage view(target.bitmapData, static_cast<int>(width), static_cast<int>(height),
              static_cast<int>(bytesPerRow), QImage::Format_RGBA8888_Premultiplied);
  return view.copy();
}

} // namespace

QImage renderMacFileIcon(const QString &path, const QSize &size) {
  if (path.isEmpty()) return {};

  @autoreleasepool {
    NSString *nsPath = [NSString stringWithUTF8String:path.toUtf8().constData()];
    if (!nsPath) return {};

    return renderNSImage([[NSWorkspace sharedWorkspace] iconForFile:nsPath], size);
  }
}

QImage renderMacSymbolIcon(const QString &name, const QSize &size) {
  if (name.isEmpty()) return {};

  @autoreleasepool {
    NSString *nsName = [NSString stringWithUTF8String:name.toUtf8().constData()];
    if (!nsName) return {};

    NSImage *symbol = [NSImage imageWithSystemSymbolName:nsName accessibilityDescription:nil];
    if (!symbol) return {};

    NSInteger const side = MIN(size.width(), size.height());
    NSImageSymbolConfiguration *config =
        [NSImageSymbolConfiguration configurationWithPointSize:side * 0.8 weight:NSFontWeightRegular];
    if (NSImage *configured = [symbol imageWithSymbolConfiguration:config]) symbol = configured;

    return renderNSImage(symbol, size);
  }
}
