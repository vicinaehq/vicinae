#include "common.hpp"
#include <qmap.h>
#include <qpixmap.h>

class IconCacheService : public NonAssignable {
  QMap<QString, QPixmap> cache;

public:
  IconCacheService() {}

  QPixmap *get(const QString &url) {
    if (auto it = cache.find(url); it != cache.end()) {
      return &*it;
    }

    return nullptr;
  }

  void set(const QString &url, QPixmap &pix) { cache.insert(url, pix); }
};
