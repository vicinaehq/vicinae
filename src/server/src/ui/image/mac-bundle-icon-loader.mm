#include "mac-bundle-icon-loader.hpp"

#import <AppKit/AppKit.h>

QImage renderMacBundleIcon(const QString &bundlePath, const QSize &size) {
  if (bundlePath.isEmpty() || !size.isValid() || size.isEmpty()) return {};

  @autoreleasepool {
    NSString *nsPath = [NSString stringWithUTF8String:bundlePath.toUtf8().constData()];
    if (!nsPath) return {};

    NSImage *icon = [[NSWorkspace sharedWorkspace] iconForFile:nsPath];
    if (!icon) return {};

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

    [NSGraphicsContext saveGraphicsState];
    [NSGraphicsContext setCurrentContext:ctx];
    [ctx setImageInterpolation:NSImageInterpolationHigh];
    [icon drawInRect:NSMakeRect(0, 0, width, height)
            fromRect:NSZeroRect
           operation:NSCompositingOperationCopy
            fraction:1.0
      respectFlipped:YES
               hints:nil];
    [NSGraphicsContext restoreGraphicsState];

    QImage view(target.bitmapData, static_cast<int>(width), static_cast<int>(height),
                static_cast<int>(bytesPerRow), QImage::Format_RGBA8888_Premultiplied);
    return view.copy();
  }
}
