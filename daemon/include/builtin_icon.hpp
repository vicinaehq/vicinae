#pragma once
#include <QString>
#include <QIcon>
#include <qpixmapcache.h>

class BuiltinIconService {
public:
  static const QList<QString> &icons();
  static QString unknownIcon() { return ":icons/question-mark-circle.svg"; }
  static QPixmap loadTinted(const QString &name, const QColor &newColor) {
    QPixmap pixmap;

    if (!QPixmapCache::find(name, &pixmap)) {
      pixmap.load(name);
      QPixmapCache::insert(name, pixmap);
    }

    QImage image = pixmap.toImage();

    // Get the RGB value for the new color
    QRgb newRgb = newColor.rgb();

    // White color in RGB (fully opaque)
    QRgb whiteRgb = qRgb(255, 255, 255);

    // Iterate through all pixels in the image
    for (int y = 0; y < image.height(); ++y) {
      for (int x = 0; x < image.width(); ++x) {
        QRgb pixelColor = image.pixel(x, y);

        // Check if the pixel is white (ignoring alpha)
        if ((pixelColor & 0x00FFFFFF) == (whiteRgb & 0x00FFFFFF)) {
          // Preserve the original alpha channel
          QRgb newColorWithAlpha = (pixelColor & 0xFF000000) | (newRgb & 0x00FFFFFF);
          image.setPixel(x, y, newColorWithAlpha);
        }
      }
    }

    // Convert back to pixmap
    return QPixmap::fromImage(image);
  }
};
