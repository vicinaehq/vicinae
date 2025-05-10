#pragma once

#include "common.hpp"
#include "network-manager.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include <QtConcurrent/qtconcurrentiteratekernel.h>
#include <algorithm>
#include <iterator>
#include <memory>
#include <numbers>
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

class FaviconImageLoader : public AbstractImageLoader {
  QSharedPointer<AbstractFaviconRequest> m_requester;
  QString m_domain;

  void render(const RenderConfig &config) override {
    auto reply = FaviconService::instance()->makeRequest(m_domain);

    m_requester = QSharedPointer<AbstractFaviconRequest>(reply);
    connect(m_requester.get(), &AbstractFaviconRequest::finished, this,
            [this, config](const QPixmap &pixmap) {
              emit dataUpdated(pixmap.scaled(config.size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            });
    connect(m_requester.get(), &AbstractFaviconRequest::failed, this,
            [this]() { emit errorOccured("Failed to load image"); });
    m_requester->start();
  }

public:
  FaviconImageLoader(const QString &hostname) : m_domain(hostname) {}
};

class QIconImageLoader : public AbstractImageLoader {
  QIcon m_icon;

public:
  void render(const RenderConfig &config) override {
    if (m_icon.isNull()) {
      emit errorOccured("No icon for this name" + m_icon.name());
      return;
    }

    auto sizes = m_icon.availableSizes();
    auto it = std::ranges::max_element(
        sizes, [](QSize a, QSize b) { return a.width() * a.height() < b.width() * b.height(); });

    // most likely SVG, we can request the size we want
    if (it == sizes.end()) {
      emit dataUpdated(m_icon.pixmap(config.size));
      return;
    }

    auto scaled = m_icon.pixmap(*it).scaled(config.size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    emit dataUpdated(scaled);
  }

  QIconImageLoader(const QString &name) {
    m_icon = QIcon(name);
    if (m_icon.isNull()) { m_icon = QIcon::fromTheme(name); }
  }
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
    if (auto fallback = m_source.fallback()) { return setUrl(*fallback); }
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

  void setUrlImpl(const OmniIconUrl &url) {
    auto &theme = ThemeService::instance().theme();
    auto type = url.type();

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
      m_loader = std::make_unique<FaviconImageLoader>(url.name());
    }

    else if (type == OmniIconType::System) {
      m_loader = std::make_unique<QIconImageLoader>(url.name());
    }

    else if (type == OmniIconType::Builtin) {
      QFile file(QString(":icons/%1.svg").arg(url.name()));

      file.open(QIODevice::ReadOnly);

      auto svgLoader = std::make_unique<SvgImageLoader>(file.readAll());

      if (m_backgroundColor) {
        QColor color = OmniPainter::textColorForBackground(*m_backgroundColor);

        svgLoader->setFillColor(OmniPainter::textColorForBackground(*m_backgroundColor));
      } else {
        svgLoader->setFillColor(url.fillColor());
      }

      m_loader = std::move(svgLoader);
    }

    else if (type == OmniIconType::Local) {
      std::filesystem::path path = url.name().toStdString();
      auto filename = path.filename().string();
      auto pos = filename.find('.');
      std::string suffixed;
      std::string suffix = "@" + theme.appearance.toStdString();

      if (pos != std::string::npos) {
        suffixed = filename.substr(0, pos) + suffix + filename.substr(pos);
      } else {
        suffixed = filename + "@dark";
      }

      std::filesystem::path suffixedPath = path.parent_path() / suffixed;

      if (std::filesystem::is_regular_file(suffixedPath)) { path = suffixedPath; }

      m_loader = std::make_unique<LocalImageLoader>(path);
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

public:
  void render() {
    if (size().isNull() || size().isEmpty() || !size().isValid()) return;

    if (!m_loader) {
      qWarning() << "No loader set for Image";
      return;
    }

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

  const OmniIconUrl &url() const { return m_source; }

  void setUrl(const OmniIconUrl &url) {
    if (url == m_source) return;
    setUrlImpl(url);
  }

  ImageWidget(QWidget *parent = nullptr) : QWidget(parent) {
    connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() { setUrlImpl(m_source); });
  }

  ~ImageWidget() {
    if (m_loader) m_loader->abort();
  }
};

}; // namespace Omnimg
