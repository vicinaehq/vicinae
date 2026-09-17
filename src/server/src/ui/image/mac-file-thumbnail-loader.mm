#include <CoreGraphics/CoreGraphics.h>
#include <QuickLookThumbnailing/QuickLookThumbnailing.h>
#include <QPromise>
#include <memory>
#include "ui/image/mac-file-thumbnail-loader.hpp"

namespace {

QImage copyThumbnail(CGImageRef source) {
  if (!source) return {};
  QImage image(static_cast<int>(CGImageGetWidth(source)), static_cast<int>(CGImageGetHeight(source)),
               QImage::Format_RGBA8888_Premultiplied);
  if (image.isNull()) return {};
  image.fill(Qt::transparent);
  CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
  CGContextRef context = CGBitmapContextCreate(
      image.bits(), image.width(), image.height(), 8, image.bytesPerLine(), colorSpace,
      static_cast<CGBitmapInfo>(kCGImageAlphaPremultipliedLast) | kCGBitmapByteOrder32Big);
  CGColorSpaceRelease(colorSpace);
  if (!context) return {};
  CGContextDrawImage(context, CGRectMake(0, 0, image.width(), image.height()), source);
  CGContextRelease(context);
  return image;
}

} // namespace

MacFileThumbnailRequest renderMacFileThumbnail(const QString &path, const QSize &size) {
  auto promise = std::make_shared<QPromise<QImage>>();
  promise->start();
  if (path.isEmpty() || size.isEmpty()) {
    promise->addResult(QImage{});
    promise->finish();
    return {promise->future(), {}};
  }
  @autoreleasepool {
    auto request = [[QLThumbnailGenerationRequest alloc]
          initWithFileAtURL:[NSURL fileURLWithPath:path.toNSString()]
                       size:CGSizeMake(size.width(), size.height())
                      scale:1.0
        representationTypes:QLThumbnailGenerationRequestRepresentationTypeThumbnail];
    [QLThumbnailGenerator.sharedGenerator
        generateBestRepresentationForRequest:request
                           completionHandler:^(QLThumbnailRepresentation *thumbnail, NSError *) {
                             promise->addResult(copyThumbnail(thumbnail.CGImage));
                             promise->finish();
                           }];
    return {promise->future(), [request] { [QLThumbnailGenerator.sharedGenerator cancelRequest:request]; }};
  }
}
