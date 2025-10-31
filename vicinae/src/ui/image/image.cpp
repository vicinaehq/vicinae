#include "image.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include "ui/image/data-uri-image-loader.hpp"
#include "ui/image/favicon-image-loader.hpp"
#include "ui/image/http-image-loader.hpp"
#include "ui/image/builtin-icon-loader.hpp"
#include "ui/image/image.hpp"
#include "ui/image/local-image-loader.hpp"
#include "ui/image/emoji-image-loader.hpp"
#include "ui/image/qicon-image-loader.hpp"
#include "ui/image/url.hpp"
#include <qlogging.h>
#include <qnamespace.h>
#include <qpainterpath.h>
#include <qpixmapcache.h>

QString ImageWidget::sizedCacheKey(const QString &key, const QSize &size) const {
  return QString("%1%2%3").arg(size.width()).arg(size.height()).arg(m_source.cacheKey());
}

void ImageWidget::handleDataUpdated(const QPixmap &data, bool cachable) {
  if (cachable && m_source.cachable()) {
    QPixmap cached;
    bool isBigger = true;
    if (QPixmapCache::find(m_source.cacheKey(), &cached); !cached.isNull()) {
      isBigger = data.width() * data.height() > cached.width() * cached.height();
    }
    if (isBigger) { QPixmapCache::insert(m_source.cacheKey(), data); }
  }
  setData(data);
}

void ImageWidget::render() {
  if (size().isNull() || size().isEmpty() || !size().isValid()) { return; }
  if (!m_loader) { return; }
  QSize drawableSize = rect().marginsRemoved(contentsMargins()).size();
  qreal pixelRatio = 1;

  if (auto sc = screen()) { pixelRatio = sc->devicePixelRatio(); }

  ++m_renderCount;

  if (m_source.cachable()) {
    QPixmap cached;

    if (QPixmapCache::find(m_source.cacheKey(), &cached);
        !cached.isNull() && cached.devicePixelRatio() == pixelRatio) {
      auto scaledDrawableSize = drawableSize * cached.devicePixelRatio();

      if (scaledDrawableSize == cached.size()) {
        setData(cached);
        return;
      }

      auto sizedKey = sizedCacheKey(m_source.cacheKey(), scaledDrawableSize);

      if (QPixmapCache::find(sizedKey, &cached)) {
        setData(cached);
        return;
      }

      bool downScalable =
          scaledDrawableSize.width() * scaledDrawableSize.height() <= cached.width() * cached.height();

      if (downScalable) {
        QPixmap scaled =
            cached.scaled(scaledDrawableSize, ImageURL::fitToAspectRatio(m_fit), Qt::SmoothTransformation);
        QPixmapCache::insert(sizedKey, scaled);
        setData(scaled);
        return;
      }
    }
  }

  m_loader->render(RenderConfig{
      .size = drawableSize, .fit = m_fit, .devicePixelRatio = pixelRatio, .fill = m_source.fillColor()});
}

void ImageWidget::setData(const QPixmap &pixmap) {
  m_data = pixmap;
  update();
}

void ImageWidget::setAlignment(Qt::Alignment alignment) {
  m_alignment = alignment;
  update();
}

void ImageWidget::setObjectFit(ObjectFit fit) {
  m_fit = fit;
  update();
}

void ImageWidget::setUrl(const ImageURL &url) {
  if (url == m_source) { return; }
  setUrlImpl(url);
}

ImageWidget::ImageWidget(QWidget *parent) : QWidget(parent) {
  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, &ImageWidget::refreshTheme);
}

ImageWidget::~ImageWidget() {
  if (m_loader) {
    disconnect(m_loader.get());
    m_loader->abort();
  }
}

const ImageURL &ImageWidget::url() const { return m_source; }

void ImageWidget::refreshTheme(const ThemeFile &theme) { setUrlImpl(m_source); }

void ImageWidget::setUrlImpl(const ImageURL &url) {
  auto &theme = ThemeService::instance().theme();
  auto type = url.type();

  m_token++; // to make sure we don't update from a previous loader
  m_loader.reset();
  m_source = url;
  m_data = {};

  if (type == ImageURLType::Favicon) {
    m_loader.reset(new FaviconImageLoader(url.name()));
  }

  else if (type == ImageURLType::System) {
    m_loader.reset(new QIconImageLoader(url.name(), url.param("theme")));
  }

  else if (type == ImageURLType::DataURI) {
    m_loader.reset(new DataUriImageLoader(QString("data:%1").arg(url.name())));
  }

  else if (type == ImageURLType::Builtin) {
    QString icon = QString(":icons/%1.svg").arg(url.name());
    auto loader = new BuiltinIconLoader(icon);

    if (url.backgroundTint()) { loader->setBackgroundColor(url.backgroundTint()); }

    m_loader.reset(loader);
  }

  else if (type == ImageURLType::Local) {
    std::filesystem::path path = url.name().toStdString();
    auto filename = path.filename().string();
    auto pos = filename.find('.');
    std::string suffixed;
    std::string suffix = "@" + std::string(theme.variant() == ThemeVariant::Dark ? "dark" : "light");

    if (pos != std::string::npos) {
      suffixed = filename.substr(0, pos) + suffix + filename.substr(pos);
    } else {
      suffixed = filename + "@dark";
    }

    std::filesystem::path suffixedPath = path.parent_path() / suffixed;
    std::error_code ec;

    if (std::filesystem::is_regular_file(suffixedPath, ec)) { path = suffixedPath; }

    m_loader.reset(new LocalImageLoader(path));
  }

  if (type == ImageURLType::Http) {
    // QUrl httpUrl("http://" + url.name());
    m_loader.reset(new HttpImageLoader(url.name()));
  }

  else if (type == ImageURLType::Emoji) {
    m_loader.reset(new EmojiImageLoader(url.name()));
  }

  if (!m_loader) { return handleLoadingError("No loader"); }

  if (m_loader) {
    connect(m_loader.get(), &AbstractImageLoader::dataUpdated, this,
            [this, token = m_token](const QPixmap &pixmap, bool cachable) {
              if (token == m_token) { handleDataUpdated(pixmap, cachable); }
            });
    connect(m_loader.get(), &AbstractImageLoader::errorOccured, this, &ImageWidget::handleLoadingError);
    if (m_renderCount > 0) { render(); }
  }
}

QSize ImageWidget::sizeHint() const {
  if (parentWidget()) { return parentWidget()->size(); }
  return {25, 25};
}

void ImageWidget::handleLoadingError(const QString &reason) {
  // qCritical() << "Failed to load" << reason;
  if (auto fallback = m_source.fallback()) { return setUrl(*fallback); }
  return setUrl(ImageURL::builtin("question-mark-circle"));
}

void ImageWidget::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  render();
}

void ImageWidget::showEvent(QShowEvent *event) {
  QWidget::showEvent(event);
  render();
}

void ImageWidget::paintEvent(QPaintEvent *event) {
  if (m_data.isNull()) { return; }

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
