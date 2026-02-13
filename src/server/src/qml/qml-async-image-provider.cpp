#include "qml-async-image-provider.hpp"
#include "favicon/favicon-service.hpp"
#include "font-service.hpp"
#include "image-fetcher.hpp"
#include "service-registry.hpp"
#include "ui/image/contrast-helper.hpp"
#include <QCoreApplication>
#include <QGuiApplication>
#include <QIcon>
#include <QImageReader>
#include <QPainter>
#include <QQuickTextureFactory>
#include <QSvgRenderer>
#include <QFutureWatcher>
#include <QThreadPool>
#include <QTimer>

// ---------------------------------------------------------------------------
// ViciImageResponse — QQuickImageResponse subclass
// ---------------------------------------------------------------------------

class ViciImageResponse : public QQuickImageResponse {
  Q_OBJECT

public:
  explicit ViciImageResponse() = default;

  QQuickTextureFactory *textureFactory() const override {
    return QQuickTextureFactory::textureFactoryForImage(m_image);
  }

  void finish(QImage image) {
    m_image = std::move(image);
    emit finished();
  }

  // Schedule finish on next event-loop iteration (for sync producers
  // that complete immediately inside requestImageResponse).
  void finishDeferred(QImage image) {
    m_image = std::move(image);
    QTimer::singleShot(0, this, &QQuickImageResponse::finished);
  }

private:
  QImage m_image;
};

// ---------------------------------------------------------------------------
// Helpers — all produce QImage, no QPixmap
// ---------------------------------------------------------------------------

static QImage renderBuiltinSvg(const QString &iconName, const QSize &size,
                                const QColor &fg, const QColor &bg) {
  QString iconPath = QStringLiteral(":icons/%1.svg").arg(iconName);
  QSvgRenderer renderer(iconPath);
  if (!renderer.isValid()) return {};

  QImage canvas(size, QImage::Format_ARGB32_Premultiplied);
  canvas.fill(Qt::transparent);
  QPainter painter(&canvas);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

  int margin = 0;

  if (bg.isValid() && bg.alpha() > 0) {
    int side = qMin(size.width(), size.height());
    qreal radius = side * 0.25;
    margin = qRound(side * 0.15);
    painter.setBrush(bg);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(canvas.rect(), radius, radius);
  }

  QRect iconRect = canvas.rect().marginsRemoved({margin, margin, margin, margin});

  // Render SVG to a temporary image so we can apply fill via CompositionMode_SourceIn
  QImage svgImage(iconRect.size(), QImage::Format_ARGB32_Premultiplied);
  svgImage.fill(Qt::transparent);
  {
    QPainter svgPainter(&svgImage);
    renderer.setAspectRatioMode(Qt::KeepAspectRatio);
    renderer.render(&svgPainter, svgImage.rect());

    QColor fillColor = fg;
    if (bg.isValid() && bg.alpha() > 0)
      fillColor = ContrastHelper::getTonalContrastColor(bg, 3);

    svgPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    svgPainter.fillRect(svgImage.rect(), fillColor);
  }

  painter.drawImage(iconRect, svgImage);
  return canvas;
}

static QImage renderEmoji(const QString &emoji, const QSize &size) {
  QImage canvas(size, QImage::Format_ARGB32_Premultiplied);
  canvas.fill(Qt::transparent);

  auto *fontService = ServiceRegistry::instance()->fontService();
  if (!fontService) return canvas;

  QFont font = fontService->emojiFont();
  font.setStyleStrategy(QFont::NoFontMerging);
  font.setPixelSize(static_cast<int>(size.height() * 0.8));

  QPainter painter(&canvas);
  painter.setFont(font);
  painter.drawText(canvas.rect(), Qt::AlignCenter, emoji);
  return canvas;
}

static QImage renderSystemIcon(const QString &name, const QSize &size) {
  QIcon icon = QIcon::fromTheme(name);
  if (icon.isNull()) return {};
  return icon.pixmap(size).toImage();
}

static QImage renderLocalSvg(const QString &path, const QSize &size) {
  QSvgRenderer renderer(path);
  if (!renderer.isValid()) return {};

  QImage canvas(size, QImage::Format_ARGB32_Premultiplied);
  canvas.fill(Qt::transparent);
  QPainter painter(&canvas);
  painter.setRenderHint(QPainter::Antialiasing, true);
  renderer.setAspectRatioMode(Qt::KeepAspectRatio);
  renderer.render(&painter, canvas.rect());
  return canvas;
}

static QImage renderLocalRaster(const QString &path, const QSize &size) {
  QImageReader reader(path);
  if (!reader.canRead()) return {};

  if (size.isValid())
    reader.setScaledSize(reader.size().scaled(size, Qt::KeepAspectRatio));

  return reader.read();
}

static QImage decodeImageData(const QByteArray &data, const QSize &size) {
  QImageReader reader;
  QBuffer *buf = new QBuffer;
  buf->setData(data);
  buf->open(QIODevice::ReadOnly);
  reader.setDevice(buf);

  if (!reader.canRead()) {
    delete buf;
    return {};
  }

  if (size.isValid()) {
    auto original = reader.size();
    if (original.isValid())
      reader.setScaledSize(original.scaled(size, Qt::KeepAspectRatio));
  }

  QImage result = reader.read();
  delete buf;
  return result;
}

static void applyCircleMask(QImage &image) {
  QImage masked(image.size(), QImage::Format_ARGB32_Premultiplied);
  masked.fill(Qt::transparent);
  QPainter painter(&masked);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setBrush(Qt::white);
  painter.setPen(Qt::NoPen);
  painter.drawEllipse(masked.rect());
  painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
  painter.drawImage(0, 0, image);
  image = masked;
}

// ---------------------------------------------------------------------------
// Param parsing
// ---------------------------------------------------------------------------

struct ParsedId {
  QString type;
  QString name;
  QColor fg;
  QColor bg;
  bool circleMask = false;
};

static ParsedId parseId(const QString &id) {
  ParsedId result;

  int colonIdx = id.indexOf(':');
  if (colonIdx < 0) return result;

  result.type = id.left(colonIdx);
  QString rest = id.mid(colonIdx + 1);

  int qmark = rest.indexOf('?');
  if (qmark < 0) {
    result.name = rest;
  } else {
    result.name = rest.left(qmark);
    QString params = rest.mid(qmark + 1);
    for (const auto &param : params.split('&')) {
      int eq = param.indexOf('=');
      if (eq < 0) continue;
      QString key = param.left(eq);
      QString val = param.mid(eq + 1);
      if (key == QStringLiteral("fg"))
        result.fg = QColor(val);
      else if (key == QStringLiteral("bg"))
        result.bg = QColor(val);
      else if (key == QStringLiteral("mask") && val == QStringLiteral("circle"))
        result.circleMask = true;
    }
  }

  return result;
}

// ---------------------------------------------------------------------------
// QmlAsyncImageProvider
// ---------------------------------------------------------------------------

QQuickImageResponse *QmlAsyncImageProvider::requestImageResponse(
    const QString &id, const QSize &requestedSize) {

  auto *response = new ViciImageResponse;
  QSize size = requestedSize.isValid() ? requestedSize : QSize(32, 32);
  auto parsed = parseId(id);

  if (parsed.type == QStringLiteral("builtin")) {
    // Builtin SVGs can be rendered on any thread (QSvgRenderer + QPainter on QImage is safe)
    QImage img = renderBuiltinSvg(parsed.name, size, parsed.fg, parsed.bg);
    if (parsed.circleMask && !img.isNull()) applyCircleMask(img);
    response->finishDeferred(std::move(img));

  } else if (parsed.type == QStringLiteral("emoji")) {
    // FontService needs main thread
    QMetaObject::invokeMethod(qApp, [response, name = parsed.name, size]() {
      QImage img = renderEmoji(name, size);
      response->finish(std::move(img));
    }, Qt::QueuedConnection);

  } else if (parsed.type == QStringLiteral("system")) {
    // QIcon::fromTheme needs main thread
    QMetaObject::invokeMethod(qApp, [response, name = parsed.name, size]() {
      QImage img = renderSystemIcon(name, size);
      response->finish(std::move(img));
    }, Qt::QueuedConnection);

  } else if (parsed.type == QStringLiteral("local")) {
    QString path = parsed.name;
    bool circle = parsed.circleMask;

    if (path.endsWith(QStringLiteral(".svg"), Qt::CaseInsensitive)) {
      QImage img = renderLocalSvg(path, size);
      if (circle && !img.isNull()) applyCircleMask(img);
      response->finishDeferred(std::move(img));
    } else {
      // Raster: decode in background thread
      QThreadPool::globalInstance()->start([response, path, size, circle]() {
        QImage img = renderLocalRaster(path, size);
        if (circle && !img.isNull()) applyCircleMask(img);
        response->finish(std::move(img));
      });
    }

  } else if (parsed.type == QStringLiteral("http")) {
    // Network fetch needs main thread for NetworkFetcher
    QString url = parsed.name;
    bool circle = parsed.circleMask;
    QMetaObject::invokeMethod(qApp, [response, url, size, circle]() {
      auto *reply = NetworkFetcher::instance()->fetch(QUrl(url));
      QObject::connect(reply, &FetchReply::finished, qApp,
                       [response, reply, size, circle](const QByteArray &data) {
        // Decode in background
        QThreadPool::globalInstance()->start([response, data, size, circle]() {
          QImage img = decodeImageData(data, size);
          if (circle && !img.isNull()) applyCircleMask(img);
          response->finish(std::move(img));
        });
        reply->deleteLater();
      });
    }, Qt::QueuedConnection);

  } else if (parsed.type == QStringLiteral("favicon")) {
    // FaviconService needs main thread
    QMetaObject::invokeMethod(qApp, [response, domain = parsed.name, size]() {
      auto *svc = FaviconService::instance();
      if (!svc) {
        response->finish({});
        return;
      }
      auto future = svc->makeRequest(domain);
      // No parent — watcher must live on main thread, response lives on reader thread
      auto *watcher = new QFutureWatcher<FaviconService::FaviconResponse>;
      QObject::connect(watcher, &QFutureWatcherBase::finished, qApp,
                       [response, watcher, size]() {
        auto result = watcher->result();
        if (result) {
          QImage img = result.value()
              .scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation)
              .toImage();
          response->finish(std::move(img));
        } else {
          response->finish({});
        }
        watcher->deleteLater();
      });
      watcher->setFuture(future);
    }, Qt::QueuedConnection);

  } else if (parsed.type == QStringLiteral("datauri")) {
    // Decode base64 data URI, then treat as local (SVG or raster)
    QString dataStr = parsed.name;
    bool circle = parsed.circleMask;
    QThreadPool::globalInstance()->start([response, dataStr, size, circle]() {
      // Parse "data:mime;base64,..."
      int commaIdx = dataStr.indexOf(',');
      if (commaIdx < 0) {
        response->finish({});
        return;
      }
      QByteArray encoded = dataStr.mid(commaIdx + 1).toUtf8();
      QByteArray decoded = QByteArray::fromBase64(encoded);

      // Check if SVG
      QString header = dataStr.left(commaIdx);
      bool isSvg = header.contains(QStringLiteral("svg"));

      QImage img;
      if (isSvg) {
        QSvgRenderer renderer(decoded);
        if (renderer.isValid()) {
          img = QImage(size, QImage::Format_ARGB32_Premultiplied);
          img.fill(Qt::transparent);
          QPainter painter(&img);
          renderer.setAspectRatioMode(Qt::KeepAspectRatio);
          renderer.render(&painter, img.rect());
        }
      } else {
        img = decodeImageData(decoded, size);
      }

      if (circle && !img.isNull()) applyCircleMask(img);
      response->finish(std::move(img));
    });

  } else {
    // Unknown type — return empty
    response->finishDeferred({});
  }

  return response;
}

#include "qml-async-image-provider.moc"
