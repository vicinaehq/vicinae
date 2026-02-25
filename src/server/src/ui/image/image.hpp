#pragma once
#include "common.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include "ui/image/url.hpp"
#include <qfont.h>
#include <qicon.h>
#include <qobject.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qsize.h>
#include <qtmetamacros.h>
#include "common/qt.hpp"

struct RenderConfig {
  QSize size;
  ObjectFit fit = ObjectFit::Contain;
  qreal devicePixelRatio = 1;

  /**
   * Fill color, if applicable. Most loaders
   * will just ignore this.
   */
  std::optional<ColorLike> fill;
};

class AbstractImageLoader : public QObject {
  Q_OBJECT

signals:
  void dataUpdated(const QPixmap &data, bool cachable = true) const;
  void errorOccured(const QString &errorDescription) const;

public:
  /**
   * Asks the loader to (re)load the image data and to start emitting
   * signals.
   */
  void virtual render(const RenderConfig &config) = 0;
  void virtual abort() const {};
  virtual ~AbstractImageLoader() = default;

  void forwardSignals(AbstractImageLoader *other) const {
    connect(this, &AbstractImageLoader::dataUpdated, other, &AbstractImageLoader::dataUpdated);
    connect(this, &AbstractImageLoader::errorOccured, other, &AbstractImageLoader::errorOccured);
  }
};
