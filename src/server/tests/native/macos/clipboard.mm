#include <AppKit/AppKit.h>
#include <QImage>
#include <catch2/catch_test_macros.hpp>
#include "services/clipboard/clipboard-mime.hpp"
#include "services/clipboard/macos/macos-pasteboard.hpp"

namespace {

struct PrivatePasteboard {
  NSPasteboard *value = [NSPasteboard pasteboardWithUniqueName];
  ~PrivatePasteboard() { [value releaseGlobally]; }
};

} // namespace

TEST_CASE("File clipboard content publishes a native file reference") {
  @autoreleasepool {
    PrivatePasteboard pasteboard;
    const auto url = QUrl::fromLocalFile("/tmp/a screenshot.png");
    auto data = Clipboard::mimeDataForContent(Clipboard::File{"/tmp/a screenshot.png"});
    REQUIRE(MacosClipboard::writePasteboard(pasteboard.value, data.release(), {}));
    REQUIRE(pasteboard.value.pasteboardItems.count == 1);
    REQUIRE(QString::fromNSString([pasteboard.value stringForType:NSPasteboardTypeFileURL]) ==
            url.toString(QUrl::FullyEncoded));
  }
}

TEST_CASE("Image-only clipboard content publishes PNG and TIFF without a source bundle") {
  @autoreleasepool {
    REQUIRE([NSBundle mainBundle].bundleIdentifier == nil);
    PrivatePasteboard pasteboard;
    QImage original(32, 16, QImage::Format_ARGB32);
    original.fill(QColor(40, 80, 120));
    auto data = std::make_unique<QMimeData>();
    data->setImageData(original);
    REQUIRE(MacosClipboard::writePasteboard(pasteboard.value, data.release(), {}));
    REQUIRE(pasteboard.value.pasteboardItems.count == 1);

    for (NSPasteboardType type in @[ NSPasteboardTypePNG, NSPasteboardTypeTIFF ]) {
      NSData *bytes = [pasteboard.value dataForType:type];
      REQUIRE(bytes.length > 0);
      const auto decoded =
          QImage::fromData(static_cast<const uchar *>(bytes.bytes), static_cast<int>(bytes.length));
      REQUIRE_FALSE(decoded.isNull());
      REQUIRE(decoded.size() == original.size());
      REQUIRE(decoded.pixelColor(0, 0) == original.pixelColor(0, 0));
    }
  }
}
