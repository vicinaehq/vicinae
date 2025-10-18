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

public:
  /**
   * Asks the loader to (re)load the image data and to start emitting
   * signals.
   */
  void virtual render(const RenderConfig &config) = 0;
  void virtual abort() const {};
  virtual ~AbstractImageLoader() {}

  void forwardSignals(AbstractImageLoader *other) const {
    connect(this, &AbstractImageLoader::dataUpdated, other, &AbstractImageLoader::dataUpdated);
    connect(this, &AbstractImageLoader::errorOccured, other, &AbstractImageLoader::errorOccured);
  }

signals:
  void dataUpdated(const QPixmap &data) const;
  void errorOccured(const QString &errorDescription) const;
};

class ImageWidget : public QWidget {
public:
  void render();
  void setAlignment(Qt::Alignment alignment);
  void setObjectFit(ObjectFit fit);
  const ImageURL &url() const;
  void setUrl(const ImageURL &url);
  ImageWidget(QWidget *parent = nullptr);
  ~ImageWidget();

private:
  QObjectUniquePtr<AbstractImageLoader> m_loader;
  QPixmap m_data;
  ImageURL m_source;
  QString m_fallback;
  int m_renderCount = 0;
  ObjectFit m_fit = ObjectFit::Contain;
  QFlags<Qt::AlignmentFlag> m_alignment = Qt::AlignCenter;
  std::optional<ColorLike> m_backgroundColor;
  int m_borderRadius = 4;

  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  void handleLoadingError(const QString &reason);
  void showEvent(QShowEvent *event) override;
  void handleDataUpdated(const QPixmap &data);
  QSize sizeHint() const override;
  void setUrlImpl(const ImageURL &url);
  void refreshTheme(const ThemeFile &theme);
};
