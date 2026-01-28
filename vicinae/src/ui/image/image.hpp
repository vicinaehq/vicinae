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
#include <qwidget.h>
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

class ImageWidget : public QWidget {
public:
  void render();
  void setAlignment(Qt::Alignment alignment);
  void setObjectFit(ObjectFit fit);
  const ImageURL &url() const;
  void setUrl(ImageURL url);
  void setData(const QPixmap &pixmap);
  ImageWidget(QWidget *parent = nullptr);
  ~ImageWidget();

private:
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  void handleLoadingError(const QString &reason);
  void showEvent(QShowEvent *event) override;
  void handleDataUpdated(const QPixmap &data, bool cachable);
  QSize sizeHint() const override;
  void setUrlImpl(ImageURL url);
  void refreshTheme(const ThemeFile &theme);
  QString sizedCacheKey(const QString &key, const QSize &size) const;

  QObjectUniquePtr<AbstractImageLoader> m_loader;
  QPixmap m_data;
  ImageURL m_source;
  int m_renderCount = 0;
  uint8_t m_token = 0;
  ObjectFit m_fit = ObjectFit::Contain;
  QFlags<Qt::AlignmentFlag> m_alignment = Qt::AlignCenter;
  std::optional<ColorLike> m_backgroundColor;
  int m_borderRadius = 4;
  bool m_isFallback = false;
};
