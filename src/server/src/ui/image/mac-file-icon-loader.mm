#include "mac-file-icon-loader.hpp"

#import <AppKit/AppKit.h>

QImage renderMacFileIcon(const QString &path, const QSize &size) {
  if (path.isEmpty() || !size.isValid() || size.isEmpty()) return {};

  @autoreleasepool {
    NSString *nsPath = [NSString stringWithUTF8String:path.toUtf8().constData()];
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

    // file icons are always square: if the size is not a square, we center the icon
    // in it and make sure we preserve the correct aspect ratio.
    NSInteger const side = MIN(width, height);
    NSRect const drawRect = NSMakeRect((width - side) / 2.0, (height - side) / 2.0, side, side);

    [NSGraphicsContext saveGraphicsState];
    [NSGraphicsContext setCurrentContext:ctx];
    [ctx setImageInterpolation:NSImageInterpolationHigh];
    [[NSColor clearColor] setFill];
    NSRectFillUsingOperation(NSMakeRect(0, 0, width, height), NSCompositingOperationCopy);
    [icon drawInRect:drawRect
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
}
