#pragma once

#include "common.hpp"
#include "data-uri/data-uri.hpp"
#include "image-fetcher.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "timer.hpp"
#include "ui/omni-painter.hpp"
#include <QtConcurrent/qtconcurrentiteratekernel.h>
#include <QtCore>
#include <algorithm>
#include <memory>
#include <qbuffer.h>
#include <qdir.h>
#include <qevent.h>
#include <qfont.h>
#include <qfuturewatcher.h>
#include <qimage.h>
#include <QtConcurrent/QtConcurrent>
#include <qimagereader.h>
#include <qlogging.h>
#include <qmimedatabase.h>
#include <qmimetype.h>
#include <qmovie.h>
#include <qnamespace.h>
#include <qnetworkreply.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qpixmap.h>
#include <filesystem>
#include <qrawfont.h>
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
  qreal devicePixelRatio = 1;
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

signals:
  void dataUpdated(const QPixmap &data) const;
  void errorOccured(const QString &errorDescription) const;
};

class AnimatedIODeviceImageLoader : public AbstractImageLoader {
  std::unique_ptr<QMovie> m_movie;
  std::unique_ptr<QIODevice> m_device = nullptr;

public:
  void render(const RenderConfig &cfg) override {
    QSize deviceSize = cfg.size * cfg.devicePixelRatio;

    m_movie = std::make_unique<QMovie>();
    m_movie->setDevice(m_device.get());

    // m_movie->setScaledSize({deviceSize.width(), -1});
    m_movie->setCacheMode(QMovie::CacheAll);
    connect(m_movie.get(), &QMovie::updated, this, [this, deviceSize, cfg]() {
      auto pix = m_movie->currentPixmap();

      QSize originalSize = pix.size();
      bool isDownScalable =
          originalSize.height() > deviceSize.height() || originalSize.width() > deviceSize.width();

      if (isDownScalable) {
        Qt::AspectRatioMode ar = cfg.fit == ObjectFitFill ? Qt::IgnoreAspectRatio : Qt::KeepAspectRatio;

        m_movie->setScaledSize(originalSize.scaled(deviceSize, ar));
        emit dataUpdated(pix.scaled(deviceSize, ar));
        return;
      }

      pix.setDevicePixelRatio(cfg.devicePixelRatio);
      emit dataUpdated(pix);
    });
    m_movie->start();
  }

  AnimatedIODeviceImageLoader(std::unique_ptr<QIODevice> device) : m_device(std::move(device)) {}
};

class StaticIODeviceImageLoader : public AbstractImageLoader {
  using ImageWatcher = QFutureWatcher<QImage>;
  QSharedPointer<ImageWatcher> m_watcher;
  std::unique_ptr<QIODevice> m_device = nullptr;

  static QImage loadStatic(std::unique_ptr<QIODevice> device, const RenderConfig &cfg) {
    QSize deviceSize = cfg.size * cfg.devicePixelRatio;
    QImageReader reader(device.get());
    QSize originalSize = reader.size();
    bool isDownScalable =
        originalSize.height() > deviceSize.height() || originalSize.width() > deviceSize.width();

    if (originalSize.isValid() && isDownScalable) {
      reader.setScaledSize(originalSize.scaled(deviceSize, cfg.fit == ObjectFitFill ? Qt::IgnoreAspectRatio
                                                                                    : Qt::KeepAspectRatio));
    }

    auto image = reader.read();

    image.setDevicePixelRatio(cfg.devicePixelRatio);

    return image;
  }

public:
  void abort() const override {
    if (m_watcher) m_watcher->cancel();
  }

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
  FetchReply *m_reply = nullptr;
  QUrl m_url;

  void handleReplyFinished() {}

  void render(const RenderConfig &cfg) override {
    QNetworkRequest request(m_url);
    // auto reply = NetworkManager::instance()->manager()->get(request);
    auto reply = NetworkFetcher::instance()->fetch(m_url);

    // important: we connect to the current reply, not m_reply
    m_reply = reply;

    connect(reply, &FetchReply::finished, this, [this, reply, cfg](const QByteArray &data) {
      auto buffer = std::make_unique<QBuffer>();

      Timer timer;
      buffer->setData(data);
      timer.time("read http image network data");
      m_loader = std::make_unique<IODeviceImageLoader>(std::move(buffer));
      connect(m_loader.get(), &IODeviceImageLoader::dataUpdated, this, &HttpImageLoader::dataUpdated);
      connect(m_loader.get(), &IODeviceImageLoader::errorOccured, this, &HttpImageLoader::errorOccured);
      m_loader->render(cfg);
      if (m_reply == reply) { m_reply = nullptr; }
      reply->deleteLater();
    });
  }

public:
  HttpImageLoader(const QUrl &url) : m_url(url) {}
  ~HttpImageLoader() {
    if (m_reply) {
      // m_reply->blockSignals(true);
      m_reply->abort();
      m_reply->deleteLater();
    }
  }
};

class LocalImageLoader : public AbstractImageLoader {
  std::unique_ptr<IODeviceImageLoader> m_loader;
  std::filesystem::path m_path;

public:
  void render(const RenderConfig &cfg) override {
    auto file = std::make_unique<QFile>(m_path);
    m_loader = std::make_unique<IODeviceImageLoader>(std::move(file));
    connect(m_loader.get(), &IODeviceImageLoader::dataUpdated, this, &LocalImageLoader::dataUpdated);
    connect(m_loader.get(), &IODeviceImageLoader::errorOccured, this, &LocalImageLoader::errorOccured);
    m_loader->render(cfg);
  }

  LocalImageLoader(const std::filesystem::path &path) { m_path = path; }
};

class DataUriImageLoader : public AbstractImageLoader {
  QTemporaryFile m_tmp;
  QObjectUniquePtr<LocalImageLoader> m_loader;

  void render(const RenderConfig &config) override { m_loader->render(config); }

public:
  DataUriImageLoader(const QString &url) {
    DataUri uri(url);

    if (!m_tmp.open()) {
      qCritical() << "Failed to open temp file for data URI image";
      return;
    }

    m_tmp.write(uri.decodeContent());
    m_tmp.close();
    m_loader.reset(new LocalImageLoader(m_tmp.filesystemFileName()));

    connect(m_loader.get(), &LocalImageLoader::dataUpdated, this, &LocalImageLoader::dataUpdated);
    connect(m_loader.get(), &LocalImageLoader::errorOccured, this, &LocalImageLoader::errorOccured);
    qDebug() << "content" << m_tmp.filesystemFileName();
  }
};

class SvgImageLoader : public AbstractImageLoader {
  QSvgRenderer m_renderer;
  std::optional<ColorLike> m_fill;

public:
  void render(QPixmap &pixmap, const QRect &bounds) {
    auto svgSize = m_renderer.defaultSize();
    // QRect targetRect = QRect(QPoint(0, 0), svgSize.scaled(bounds.size(), Qt::KeepAspectRatio));

    QPixmap filledSvg(bounds.size());

    filledSvg.fill(Qt::transparent);

    // first, we paint the filled svg on a separate pixmap
    {
      OmniPainter painter(&filledSvg);

      m_renderer.render(&painter, filledSvg.rect());

      if (m_fill) {
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.fillRect(filledSvg.rect(), *m_fill);
      }
    }

    QPainter painter(&pixmap);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.drawPixmap(bounds, filledSvg);
  }

  void render(const RenderConfig &config) override {
    QPixmap pixmap(config.size * config.devicePixelRatio);

    render(pixmap, pixmap.rect());
    pixmap.setDevicePixelRatio(config.devicePixelRatio);
    emit dataUpdated(pixmap);
  }

  void setFillColor(const std::optional<ColorLike> &color) { m_fill = color; }

  SvgImageLoader(const QByteArray &data) { m_renderer.load(data); }
  SvgImageLoader(const QString &filename) { m_renderer.load(filename); }
};

class FaviconImageLoader : public AbstractImageLoader {
  QSharedPointer<AbstractFaviconRequest> m_requester;
  QString m_domain;

  void render(const RenderConfig &config) override {
    auto reply = FaviconService::instance()->makeRequest(m_domain);

    m_requester = QSharedPointer<AbstractFaviconRequest>(reply);
    connect(m_requester.get(), &AbstractFaviconRequest::finished, this,
            [this, config](const QPixmap &pixmap) {
              auto scaled = pixmap.scaled(config.size * config.devicePixelRatio, Qt::KeepAspectRatio,
                                          Qt::SmoothTransformation);

              scaled.setDevicePixelRatio(config.devicePixelRatio);
              emit dataUpdated(scaled);
            });
    connect(m_requester.get(), &AbstractFaviconRequest::failed, this,
            [this]() { emit errorOccured("Failed to load image"); });
    m_requester->start();
  }

public:
  FaviconImageLoader(const QString &hostname) : m_domain(hostname) {}
};

class EmojiImageLoader : public AbstractImageLoader {
  QString m_emoji;

  void render(const RenderConfig &config) override {
    auto font = QFont("Twemoji");
    QPixmap canva(config.size * config.devicePixelRatio);

    font.setStyleStrategy(QFont::StyleStrategy::NoFontMerging);

    canva.fill(Qt::transparent);
    font.setPixelSize(canva.height() * 0.8);

    QPainter painter(&canva);

    painter.setFont(font);
    painter.drawText(canva.rect(), Qt::AlignCenter, m_emoji);
    canva.setDevicePixelRatio(config.devicePixelRatio);

    emit dataUpdated(canva);
  }

public:
  EmojiImageLoader(const QString &emoji) : m_emoji(emoji) {}
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

    auto pix =
        m_icon.pixmap(config.size)
            .scaled(config.size * config.devicePixelRatio, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    pix.setDevicePixelRatio(config.devicePixelRatio);

    emit dataUpdated(pix);
  }

  QIconImageLoader(const QString &name) {
    m_icon = QIcon(name);
    if (m_icon.isNull()) { m_icon = QIcon::fromTheme(name); }
  }
};

class BuiltinIconLoader : public AbstractImageLoader {
  std::optional<ColorLike> m_backgroundColor;
  std::optional<ColorLike> m_fillColor;
  QString m_iconName;

  void render(const RenderConfig &config) override {
    QPixmap canva(config.size * config.devicePixelRatio);
    int margin = 0;

    canva.fill(Qt::transparent);

    if (m_backgroundColor) {
      OmniPainter painter(&canva);
      qreal radius = qRound(4 * config.devicePixelRatio);

      painter.setRenderHint(QPainter::Antialiasing, true);
      margin = qRound(3 * config.devicePixelRatio);
      painter.setBrush(painter.colorBrush(*m_backgroundColor));
      painter.setPen(Qt::NoPen);
      painter.drawRoundedRect(canva.rect(), radius, radius);
    }

    QMargins margins{margin, margin, margin, margin};
    QRect iconRect = canva.rect().marginsRemoved(margins);
    SvgImageLoader loader(m_iconName);

    loader.setFillColor(m_fillColor);
    loader.render(canva, iconRect);
    canva.setDevicePixelRatio(config.devicePixelRatio);
    emit dataUpdated(canva);
  }

public:
  void setFillColor(const std::optional<ColorLike> &color) { m_fillColor = color; }
  void setBackgroundColor(const std::optional<ColorLike> &color) { m_backgroundColor = color; }

  BuiltinIconLoader(const QString &iconName) : m_iconName(iconName) {}
};

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

    auto logicalDataSize = m_data.size() / m_data.devicePixelRatio();
    int horizontalMargins = width() - logicalDataSize.width();
    int verticalMargins = height() - logicalDataSize.height();
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

    QPainterPath path;

    switch (m_source.mask()) {
    case OmniPainter::ImageMaskType::CircleMask:
      path.addEllipse(rect());
      painter.setClipPath(path);
      break;
    case OmniPainter::ImageMaskType::RoundedRectangleMask:
      path.addRoundedRect(rect(), 6, 6);
      painter.setClipPath(path);
      break;
    default:
      break;
    }

    painter.drawPixmap(pos, m_data);
  }

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    render();
  }

  void handleLoadingError(const QString &reason) {
    // qCritical() << "Failed to load" << reason;
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
    m_data = {};
    m_loader.reset();

    if (type == OmniIconType::Favicon) {
      m_loader = std::make_unique<FaviconImageLoader>(url.name());
    }

    else if (type == OmniIconType::System) {
      m_loader = std::make_unique<QIconImageLoader>(url.name());
    }

    else if (type == OmniIconType::DataURI) {
      m_loader = std::make_unique<DataUriImageLoader>(QString("data:%1").arg(url.name()));
    }

    else if (type == OmniIconType::Builtin) {
      QString icon = QString(":icons/%1.svg").arg(url.name());
      auto loader = std::make_unique<BuiltinIconLoader>(icon);

      if (url.backgroundTint()) {
        loader->setBackgroundColor(url.backgroundTint());
        loader->setFillColor(OmniPainter::textColorForBackground(url.backgroundTint()));
      } else {
        loader->setFillColor(url.fillColor());
      }

      m_loader = std::move(loader);
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
      m_loader = std::make_unique<EmojiImageLoader>(url.name());
    }

    if (!m_loader) { return handleLoadingError("No loader"); }

    if (m_loader) {
      connect(m_loader.get(), &AbstractImageLoader::dataUpdated, this, &ImageWidget::handleDataUpdated);
      connect(m_loader.get(), &AbstractImageLoader::errorOccured, this, &ImageWidget::handleLoadingError);
      if (m_renderCount > 0) render();
    }
  }

  void refreshTheme(const ThemeInfo &theme) { setUrlImpl(m_source); }

public:
  void render() {
    if (size().isNull() || size().isEmpty() || !size().isValid()) return;

    if (!m_loader) { return; }

    QSize drawableSize = rect().marginsRemoved(contentsMargins()).size();

    m_renderCount += 1;
    m_loader->render(
        RenderConfig{.size = drawableSize, .fit = m_fit, .devicePixelRatio = qApp->devicePixelRatio()});
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
    if (url == m_source) { return; }
    setUrlImpl(url);
  }

  ImageWidget(QWidget *parent = nullptr) : QWidget(parent) {
    connect(&ThemeService::instance(), &ThemeService::themeChanged, this, &ImageWidget::refreshTheme);
  }

  ~ImageWidget() {
    if (m_loader) {
      disconnect(m_loader.get());
      m_loader->abort();
    }
  }
};

}; // namespace Omnimg
