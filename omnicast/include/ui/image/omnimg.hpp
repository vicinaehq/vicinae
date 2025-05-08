#pragma once

#include "common.hpp"
#include "network-manager.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include <QtConcurrent/qtconcurrentiteratekernel.h>
#include <iterator>
#include <memory>
#include <qbuffer.h>
#include <qdir.h>
#include <qevent.h>
#include <qfuturewatcher.h>
#include <qimage.h>
#include <QtConcurrent/QtConcurrent>
#include <qimagereader.h>
#include <qlogging.h>
#include <qmimedatabase.h>
#include <qmovie.h>
#include <qnamespace.h>
#include <qnetworkreply.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <filesystem>
#include <qsharedpointer.h>
#include <qstringview.h>
#include <qsvgrenderer.h>
#include <qurl.h>
#include <qwidget.h>

namespace Omnimg {
enum ObjectFit { ObjectFitContain, ObjectFitFill };

struct RenderConfig {
  QSize size;
  ObjectFit fit = ObjectFitContain;
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

signals:
  void dataUpdated(const QPixmap &data) const;
  void errorOccured(const QString &errorDescription) const;
};

class AnimatedIODeviceImageLoader : public AbstractImageLoader {
  std::unique_ptr<QMovie> m_movie;
  std::unique_ptr<QIODevice> m_device = nullptr;

public:
  void render(const RenderConfig &cfg) override {
    m_movie = std::make_unique<QMovie>();
    m_movie->setDevice(m_device.get());
    m_movie->setScaledSize(cfg.size);
    m_movie->setCacheMode(QMovie::CacheAll);
    connect(m_movie.get(), &QMovie::updated, this, [this]() { emit dataUpdated(m_movie->currentPixmap()); });
    m_movie->start();
  }

  AnimatedIODeviceImageLoader(std::unique_ptr<QIODevice> device) : m_device(std::move(device)) {}
};

class StaticIODeviceImageLoader : public AbstractImageLoader {
  using ImageWatcher = QFutureWatcher<QImage>;
  QSharedPointer<ImageWatcher> m_watcher;
  std::unique_ptr<QIODevice> m_device = nullptr;

  static QImage loadStatic(std::unique_ptr<QIODevice> device, const RenderConfig &cfg) {
    QImageReader reader(device.get());
    QSize originalSize = reader.size();
    bool isDownScalable =
        originalSize.height() > cfg.size.height() || originalSize.width() > cfg.size.width();

    if (originalSize.isValid() && isDownScalable) {
      reader.setScaledSize(originalSize.scaled(cfg.size, cfg.fit == ObjectFitFill ? Qt::IgnoreAspectRatio
                                                                                  : Qt::KeepAspectRatio));
    }

    return reader.read();
  }

public:
  void abort() const override { m_watcher->cancel(); }

  void render(const RenderConfig &cfg) override {
    auto watcher = QSharedPointer<ImageWatcher>::create();
    auto future = QtConcurrent::run([cfg, this]() { return loadStatic(std::move(m_device), cfg); });

    m_watcher = watcher;
    watcher->setFuture(future);
    connect(watcher.get(), &ImageWatcher::finished, this,
            [this, watcher]() { emit dataUpdated(QPixmap::fromImage(watcher->future().takeResult())); });
  }

public:
  StaticIODeviceImageLoader(std::unique_ptr<QIODevice> device) : m_device(std::move(device)) {}
  ~StaticIODeviceImageLoader() {
    if (m_watcher && m_watcher->isRunning()) { m_watcher->cancel(); }
  }
};

class IODeviceImageLoader : public AbstractImageLoader {
  std::unique_ptr<QIODevice> m_device;
  std::unique_ptr<AbstractImageLoader> m_loader;

  bool isAnimatableMimeType(const QMimeType &type) const { return type.name() == "image/gif"; }

public:
  void render(const RenderConfig &cfg) override {
    if (!m_device->isOpen()) {
      if (!m_device->open(QIODevice::ReadOnly)) {
        emit errorOccured(QString("IODevice could not be opened for reading"));
        return;
      } else {
        qWarning() << "Opened device";
      }
    }

    QMimeDatabase mimeDb;
    QMimeType mime = mimeDb.mimeTypeForData(m_device.get());

    if (isAnimatableMimeType(mime)) {
      m_loader = std::make_unique<AnimatedIODeviceImageLoader>(std::move(m_device));
    } else {
      m_loader = std::make_unique<StaticIODeviceImageLoader>(std::move(m_device));
    }

    connect(m_loader.get(), &AbstractImageLoader::dataUpdated, this, &IODeviceImageLoader::dataUpdated);
    connect(m_loader.get(), &AbstractImageLoader::errorOccured, this, &IODeviceImageLoader::errorOccured);
    m_loader->render(cfg);
  }

  IODeviceImageLoader(std::unique_ptr<QIODevice> device) : m_device(std::move(device)) {}
};

class HttpImageLoader : public AbstractImageLoader {
  std::unique_ptr<IODeviceImageLoader> m_loader;
  QObjectUniquePtr<QNetworkReply> m_reply;
  QUrl m_url;

  void handleReplyFinished() {}

  void render(const RenderConfig &cfg) override {
    QNetworkRequest m_request(m_url);
    auto reply = NetworkManager::instance()->manager()->get(m_request);

    m_reply = QObjectUniquePtr<QNetworkReply>(reply);
    connect(m_reply.get(), &QNetworkReply::finished, this, [this, cfg]() {
      auto buffer = std::make_unique<QBuffer>();

      buffer->setData(m_reply->readAll());
      m_loader = std::make_unique<IODeviceImageLoader>(std::move(buffer));
      connect(m_loader.get(), &IODeviceImageLoader::dataUpdated, this, &HttpImageLoader::dataUpdated);
      connect(m_loader.get(), &IODeviceImageLoader::errorOccured, this, &HttpImageLoader::errorOccured);
      m_loader->render(cfg);
    });
  }

public:
  HttpImageLoader(const QUrl &url) : m_url(url) {}

  ~HttpImageLoader() {}
};

class LocalImageLoader : public AbstractImageLoader {
  std::unique_ptr<IODeviceImageLoader> m_loader;
  std::filesystem::path m_path;

  void render(const RenderConfig &cfg) override {
    auto file = std::make_unique<QFile>(m_path);
    qDebug() << "render with" << file->fileName();
    m_loader = std::make_unique<IODeviceImageLoader>(std::move(file));
    connect(m_loader.get(), &IODeviceImageLoader::dataUpdated, this, &LocalImageLoader::dataUpdated);
    connect(m_loader.get(), &IODeviceImageLoader::errorOccured, this, &LocalImageLoader::errorOccured);
    m_loader->render(cfg);
  }

public:
  LocalImageLoader(const std::filesystem::path &path) { m_path = path; }

  ~LocalImageLoader() {}
};

class SvgImageLoader : public AbstractImageLoader {
  QSvgRenderer m_renderer;
  std::optional<ColorLike> m_fill;

  void render(const RenderConfig &config) override {
    QPixmap pixmap(config.size);

    pixmap.fill(Qt::transparent);
    OmniPainter painter(&pixmap);

    m_renderer.render(&painter);

    if (m_fill) {
      painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
      painter.fillRect(pixmap.rect(), *m_fill);
    }

    pixmap.setDevicePixelRatio(1);

    emit dataUpdated(pixmap);
  }

public:
  void setFillColor(const std::optional<ColorLike> &color) { m_fill = color; }

  SvgImageLoader(const QByteArray &data) { m_renderer.load(data); }
};

// for now, hardcoded to only work with local images, will expand later on, no worries
class ImageWidget : public QWidget {
  std::unique_ptr<AbstractImageLoader> m_loader;
  QPixmap m_data;
  OmniIconUrl m_source;
  QString m_fallback;
  int m_renderCount = 0;
  ObjectFit m_fit = ObjectFit::ObjectFitContain;
  QFlags<Qt::AlignmentFlag> m_alignment = Qt::AlignCenter;
  std::optional<ColorLike> m_backgroundColor;
  int m_borderRadius = 4;

  void paintEvent(QPaintEvent *event) override {
    if (m_data.isNull()) return;

    auto dataLogicalSize = m_data.size() / devicePixelRatio();
    int horizontalMargins = width() - dataLogicalSize.width();
    int verticalMargins = height() - dataLogicalSize.height();
    QPoint pos(0, 0);

    if (m_alignment.testFlag(Qt::AlignRight)) { pos.setX(horizontalMargins); }
    if (m_alignment.testFlag(Qt::AlignBottom)) { pos.setY(verticalMargins); }
    if (m_alignment.testFlag(Qt::AlignHCenter)) { pos.setX(horizontalMargins / 2); }
    if (m_alignment.testFlag(Qt::AlignVCenter)) { pos.setY(verticalMargins / 2); }

    OmniPainter painter(this);

    painter.setClipRegion(event->region());
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);

    if (m_backgroundColor) {
      painter.setPen(Qt::NoPen);
      painter.setBrush(painter.colorBrush(*m_backgroundColor));
      painter.drawRoundedRect(rect(), m_borderRadius, m_borderRadius);
    }

    painter.drawPixmap(pos, m_data);
  }

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    render();
  }

  void handleLoadingError(const QString &reason) {
    qCritical() << "Failed to load" << reason;
    if (auto fallback = m_source.fallback()) {
      qDebug() << "Fall back on" << *fallback;
      return setUrl(*fallback);
    }
    return setUrl(BuiltinOmniIconUrl("question-mark-circle"));
  }

  void handleDataUpdated(const QPixmap &data) {
    m_data = data;
    update();
  }

  QSize sizeHint() const override {
    if (parentWidget()) { return parentWidget()->size(); }
    return {25, 25};
  }

public:
  void render() {
    if (size().isNull() || size().isEmpty() || !size().isValid()) return;

    if (!m_loader) {
      qWarning() << "No loader set for Image";
      return;
    }

    qDebug() << "Rendering for size" << size();
    // we always provide the renderer with the scaled size, so that loading can be
    // optimized properly for highDPI.
    auto drawingRect = rect().marginsRemoved(contentsMargins());
    auto deviceSize = drawingRect.size() * devicePixelRatio();

    m_renderCount += 1;
    m_loader->render(RenderConfig{.size = deviceSize, .fit = m_fit});
  }

  void setAlignment(Qt::Alignment alignment) {
    m_alignment = alignment;
    update();
  }

  void setObjectFit(ObjectFit fit) {
    m_fit = fit;
    update();
  }

  void setUrl(const OmniIconUrl &url) {
    if (url == m_source) return;
    auto type = url.type();

    qDebug() << "loading" << url.toString();

    m_source = url;
    m_backgroundColor = {};
    m_data = {};
    m_loader.reset();
    setContentsMargins(0, 0, 0, 0);

    if (url.backgroundTint() != ColorTint::InvalidTint) {
      m_backgroundColor = url.backgroundTint();
      setContentsMargins(3, 3, 3, 3);
    }

    if (type == OmniIconType::Favicon) {
      // XXX - TBD
    }

    else if (type == OmniIconType::System) {
      // XXX - TBD
    }

    else if (type == OmniIconType::Builtin) {
      QFile file(QString(":icons/%1.svg").arg(url.name()));

      file.open(QIODevice::ReadOnly);

      auto svgLoader = std::make_unique<SvgImageLoader>(file.readAll());

      svgLoader->setFillColor(url.fillColor());
      m_loader = std::move(svgLoader);
    }

    else if (type == OmniIconType::Local) {
      qDebug() << "local icon" << url.name();
      m_loader = std::make_unique<LocalImageLoader>(url.name().toStdString());
    }

    else if (type == OmniIconType::Http) {
      QUrl httpUrl("https://" + url.name());

      m_loader = std::make_unique<HttpImageLoader>(httpUrl);
    }

    else if (type == OmniIconType::Emoji) {
      // XXX - TBD
    }

    if (!m_loader) { return handleLoadingError("No loader"); }

    if (m_loader) {
      connect(m_loader.get(), &AbstractImageLoader::dataUpdated, this, &ImageWidget::handleDataUpdated);
      connect(m_loader.get(), &AbstractImageLoader::errorOccured, this, &ImageWidget::handleLoadingError);
      if (m_renderCount > 0) render();
    }
  }

  ~ImageWidget() {
    if (m_loader) m_loader->abort();
    qCritical() << "Remove image!" << m_source;
  }
};

}; // namespace Omnimg
