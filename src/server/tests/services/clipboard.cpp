#include <QImage>
#include <catch2/catch_test_macros.hpp>
#include "services/clipboard/clipboard-mime.hpp"

TEST_CASE("Image clipboard content preserves pixels and does not become a file reference") {
  QImage image(32, 16, QImage::Format_ARGB32);
  image.fill(QColor(40, 80, 120, 128));
  auto mime = Clipboard::mimeDataForContent(Clipboard::Image{image});
  REQUIRE(mime);
  REQUIRE(mime->hasImage());
  REQUIRE_FALSE(mime->hasUrls());
  REQUIRE(qvariant_cast<QImage>(mime->imageData()) == image);

  const auto selection = Clipboard::selectionFromMimeData(mime.get());
  REQUIRE(selection);
  REQUIRE(selection->offers.size() == 1);
  REQUIRE(selection->offers.front().mimeType == "image/png");
  REQUIRE(QImage::fromData(selection->offers.front().data) == image);
}

TEST_CASE("Invalid image content is rejected while copying a file keeps file semantics") {
  REQUIRE_FALSE(Clipboard::mimeDataForContent(Clipboard::Image{}));
  const auto mime = Clipboard::mimeDataForContent(Clipboard::File{"/tmp/a screenshot.png"});
  REQUIRE(mime->hasUrls());
  REQUIRE_FALSE(mime->hasImage());
  REQUIRE(mime->urls().front() == QUrl::fromLocalFile("/tmp/a screenshot.png"));
}
