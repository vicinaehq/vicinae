#include "image.hpp"
#include "ui/image/data-uri-image-loader.hpp"
#include "ui/image/favicon-image-loader.hpp"
#include "ui/image/http-image-loader.hpp"
#include "ui/image/builtin-icon-loader.hpp"
#include "ui/image/image.hpp"
#include "ui/image/local-image-loader.hpp"
#include "ui/image/emoji-image-loader.hpp"
#include "ui/image/qicon-image-loader.hpp"
#include <qpainterpath.h>

void ImageWidget::handleDataUpdated(const QPixmap &data) {
  m_data = data;
  update();
}

void ImageWidget::render() {
  if (size().isNull() || size().isEmpty() || !size().isValid()) return;

  if (!m_loader) { return; }

  QSize drawableSize = rect().marginsRemoved(contentsMargins()).size();

  m_renderCount += 1;
  m_loader->render(
      RenderConfig{.size = drawableSize, .fit = m_fit, .devicePixelRatio = qApp->devicePixelRatio()});
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

void ImageWidget::refreshTheme(const ThemeInfo &theme) { setUrlImpl(m_source); }

void ImageWidget::setUrlImpl(const ImageURL &url) {
  auto &theme = ThemeService::instance().theme();
  auto type = url.type();

  m_source = url;
  m_data = {};
  m_loader.reset();

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

    if (url.backgroundTint()) {
      loader->setBackgroundColor(url.backgroundTint());
      loader->setFillColor(SemanticColor::TextOnAccent);
    } else {
      loader->setFillColor(url.fillColor());
    }

    m_loader.reset(loader);
  }

  else if (type == ImageURLType::Local) {
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

    m_loader.reset(new LocalImageLoader(path));
  }

  else if (type == ImageURLType::Http) {
    QUrl httpUrl("https://" + url.name());

    m_loader.reset(new HttpImageLoader(httpUrl));
  }

  else if (type == ImageURLType::Emoji) {
    m_loader.reset(new EmojiImageLoader(url.name()));
  }

  if (!m_loader) { return handleLoadingError("No loader"); }

  if (m_loader) {
    connect(m_loader.get(), &AbstractImageLoader::dataUpdated, this, &ImageWidget::handleDataUpdated);
    connect(m_loader.get(), &AbstractImageLoader::errorOccured, this, &ImageWidget::handleLoadingError);
    if (m_renderCount > 0) render();
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
