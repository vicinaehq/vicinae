#include "qml-image-url.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include "ui/omni-painter/omni-painter.hpp"

QmlImageUrl::QmlImageUrl(ImageURL url) : m_url(std::move(url)) {}

const ImageURL &QmlImageUrl::imageUrl() const { return m_url; }

bool QmlImageUrl::isValid() const { return m_url.type() != ImageURLType::Invalid; }

bool QmlImageUrl::isThemeSensitive() const {
  auto t = m_url.type();
  if (t == ImageURLType::Builtin || t == ImageURLType::Local)
    return true;
  return m_url.fillColor().has_value();
}

static QColor resolveFillColor(const ImageURL &url) {
  auto fill = url.fillColor();
  if (!fill) return {};

  auto &theme = ThemeService::instance().theme();
  if (auto *sc = std::get_if<SemanticColor>(&*fill))
    return theme.resolve(*sc);
  if (auto *qc = std::get_if<QColor>(&*fill))
    return *qc;
  if (auto *str = std::get_if<QString>(&*fill))
    return QColor(*str);
  return {};
}

static QString buildProviderIdForFallback(const ImageURL &url);

static QString buildParams(const ImageURL &url, bool builtinFgDefault = false) {
  QStringList parts;

  QColor fg = resolveFillColor(url);
  if (builtinFgDefault && !fg.isValid())
    fg = ThemeService::instance().theme().resolve(SemanticColor::Foreground);
  if (fg.isValid())
    parts << QStringLiteral("fg=") + fg.name(QColor::HexRgb);

  if (auto bgTint = url.backgroundTint()) {
    QColor bg = ThemeService::instance().theme().resolve(*bgTint);
    parts << QStringLiteral("bg=") + bg.name(QColor::HexRgb);
  }

  if (url.mask() == OmniPainter::CircleMask)
    parts << QStringLiteral("mask=circle");

  if (auto fb = url.fallback()) {
    ImageURL fbUrl(*fb);
    QString fbId = buildProviderIdForFallback(fbUrl);
    if (!fbId.isEmpty())
      parts << QStringLiteral("fallback=") + QString::fromUtf8(QUrl::toPercentEncoding(fbId));
  }

  if (parts.isEmpty()) return {};
  return QStringLiteral("?") + parts.join(QStringLiteral("&"));
}

static QString buildProviderIdForFallback(const ImageURL &url) {
  QString source = QmlImageUrl(url).toSource();
  static const QString prefix = QStringLiteral("image://vicinae/");
  if (source.startsWith(prefix))
    return source.mid(prefix.length());
  return {};
}

QString QmlImageUrl::toSource() const {
  if (!isValid()) return {};

  QString prefix = QStringLiteral("image://vicinae/");
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
  case ImageURLType::Https:
    return prefix + QStringLiteral("http:") + name + buildParams(m_url);

  case ImageURLType::Emoji:
    return prefix + QStringLiteral("emoji:") + name;

  case ImageURLType::Favicon:
    return prefix + QStringLiteral("favicon:") + name;

  case ImageURLType::DataURI:
    return prefix + QStringLiteral("datauri:") + name + buildParams(m_url);

  default:
    return {};
  }
}

QmlImageUrl QmlImageUrl::withFallback(const QmlImageUrl &fb) const {
  ImageURL copy = m_url;
  copy.withFallback(fb.m_url);
  return QmlImageUrl(std::move(copy));
}

QmlImageUrl QmlImageUrl::withBackgroundTint(const QString &tint) const {
  ImageURL copy = m_url;
  auto color = ImageURL::tintForName(tint);
  if (color != SemanticColor::InvalidTint)
    copy.setBackgroundTint(color);
  return QmlImageUrl(std::move(copy));
}

QmlImageUrl QmlImageUrl::withFillColor(const QColor &color) const {
  ImageURL copy = m_url;
  copy.setFill(color);
  return QmlImageUrl(std::move(copy));
}

QmlImageUrl QmlImageUrl::withCircleMask() const {
  ImageURL copy = m_url;
  copy.circle();
  return QmlImageUrl(std::move(copy));
}
