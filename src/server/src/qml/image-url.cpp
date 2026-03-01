#include "image-url.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include "ui/omni-painter/omni-painter.hpp"

ImageUrl::ImageUrl(ImageURL url) : m_url(std::move(url)) {}

const ImageURL &ImageUrl::imageUrl() const { return m_url; }

bool ImageUrl::isValid() const { return m_url.type() != ImageURLType::Invalid; }

bool ImageUrl::isThemeSensitive() const {
  auto t = m_url.type();
  if (t == ImageURLType::Builtin || t == ImageURLType::Local) return true;
  return m_url.fillColor().has_value();
}

static QColor resolveFillColor(const ImageURL &url) {
  auto fill = url.fillColor();
  if (!fill) return {};

  auto &theme = ThemeService::instance().theme();
  if (auto *sc = std::get_if<SemanticColor>(&*fill)) return theme.resolve(*sc);
  if (auto *qc = std::get_if<QColor>(&*fill)) return *qc;
  if (auto *str = std::get_if<QString>(&*fill)) return QColor(*str);
  return {};
}

static QString buildProviderIdForFallback(const ImageURL &url);

static QString buildParams(const ImageURL &url, bool builtinFgDefault = false) {
  QStringList parts;

  QColor fg = resolveFillColor(url);
  if (builtinFgDefault && !fg.isValid())
    fg = ThemeService::instance().theme().resolve(SemanticColor::Foreground);
  if (fg.isValid()) {
    auto fmt = fg.alphaF() < 1.0 ? QColor::HexArgb : QColor::HexRgb;
    parts << QStringLiteral("fg=%23") + fg.name(fmt).mid(1);
  }

  if (auto bgTint = url.backgroundTint()) {
    QColor const bg = ThemeService::instance().theme().resolve(*bgTint);
    parts << QStringLiteral("bg=%23") + bg.name(QColor::HexRgb).mid(1);
  }

  if (url.mask() == OmniPainter::CircleMask) parts << QStringLiteral("mask=circle");

  if (auto fb = url.fallback()) {
    ImageURL const fbUrl(*fb);
    QString const fbId = buildProviderIdForFallback(fbUrl);
    if (!fbId.isEmpty())
      parts << QStringLiteral("fallback=") + QString::fromUtf8(QUrl::toPercentEncoding(fbId));
  }

  if (parts.isEmpty()) return {};
  return QStringLiteral("?") + parts.join(QStringLiteral("&"));
}

static QString buildProviderIdForFallback(const ImageURL &url) {
  QString const source = ImageUrl(url).toSource();
  static const QString prefix = QStringLiteral("image://vicinae/");
  if (source.startsWith(prefix)) return source.mid(prefix.length());
  return {};
}

QString ImageUrl::toSource() const {
  if (!isValid()) return {};

  QString const prefix = QStringLiteral("image://vicinae/");
  auto type = m_url.type();
  const QString &name = m_url.name();

  switch (type) {
  case ImageURLType::Builtin:
    return prefix + QStringLiteral("builtin:") + name + buildParams(m_url, true);

  case ImageURLType::System:
    return prefix + QStringLiteral("system:") + name + buildParams(m_url);

  case ImageURLType::Local:
    return prefix + QStringLiteral("local:") + name + buildParams(m_url);

  case ImageURLType::Http:
  case ImageURLType::Https: {
    QString params = buildParams(m_url);
    if (!params.isEmpty()) {
      params[0] = ';';
      params.replace('&', ';');
    }
    return prefix + QStringLiteral("http") + params + QStringLiteral(":") + name;
  }

  case ImageURLType::Emoji:
    return prefix + QStringLiteral("emoji:") + name;

  case ImageURLType::Favicon:
    return prefix + QStringLiteral("favicon:") + name;

  case ImageURLType::DataURI: {
    QString params = buildParams(m_url);
    if (!params.isEmpty()) {
      params[0] = ';';
      params.replace('&', ';');
    }
    return prefix + QStringLiteral("datauri") + params + QStringLiteral(":") + name;
  }

  default:
    return {};
  }
}

ImageUrl ImageUrl::withFallback(const ImageUrl &fb) const {
  ImageURL copy = m_url;
  copy.withFallback(fb.m_url);
  return ImageUrl(std::move(copy));
}

ImageUrl ImageUrl::withBackgroundTint(const QString &tint) const {
  ImageURL copy = m_url;
  auto color = ImageURL::tintForName(tint);
  if (color != SemanticColor::InvalidTint) copy.setBackgroundTint(color);
  return ImageUrl(std::move(copy));
}

ImageUrl ImageUrl::withFillColor(const QColor &color) const {
  ImageURL copy = m_url;
  copy.setFill(color);
  return ImageUrl(std::move(copy));
}

ImageUrl ImageUrl::withCircleMask() const {
  ImageURL copy = m_url;
  copy.circle();
  return ImageUrl(std::move(copy));
}
