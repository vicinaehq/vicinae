#include "qml-image-url.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include "ui/omni-painter/omni-painter.hpp"

QmlImageUrl::QmlImageUrl(ImageURL url) : m_url(std::move(url)) {}

const ImageURL &QmlImageUrl::imageUrl() const { return m_url; }

bool QmlImageUrl::isValid() const { return m_url.type() != ImageURLType::Invalid; }

bool QmlImageUrl::isThemeSensitive() const {
  auto t = m_url.type();
  return t == ImageURLType::Builtin || t == ImageURLType::Local;
}

QString QmlImageUrl::toSource() const {
  if (!isValid()) return {};

  QString prefix = QStringLiteral("image://vicinae/");
  auto type = m_url.type();
  const QString &name = m_url.name();

  switch (type) {
  case ImageURLType::Builtin: {
    // Eagerly resolve the current theme's foreground color so the provider
    // receives a concrete hex color (no theme access needed on image thread).
    // When the theme changes, QML re-evaluates the binding → toSource()
    // returns a different URL → cache miss → fresh render.
    auto &theme = ThemeService::instance().theme();
    QColor fg;
    if (auto fill = m_url.fillColor()) {
      if (auto *sc = std::get_if<SemanticColor>(&*fill))
        fg = theme.resolve(*sc);
      else if (auto *qc = std::get_if<QColor>(&*fill))
        fg = *qc;
      else if (auto *str = std::get_if<QString>(&*fill))
        fg = QColor(*str);
    }
    if (!fg.isValid()) fg = theme.resolve(SemanticColor::Foreground);
    QString params = QStringLiteral("?fg=") + fg.name(QColor::HexRgb);

    if (auto bgTint = m_url.backgroundTint()) {
      QColor bg = theme.resolve(*bgTint);
      params += QStringLiteral("&bg=") + bg.name(QColor::HexRgb);
    }

    if (m_url.mask() == OmniPainter::CircleMask)
      params += QStringLiteral("&mask=circle");

    return prefix + QStringLiteral("builtin:") + name + params;
  }

  case ImageURLType::System:
    return prefix + QStringLiteral("system:") + name;

  case ImageURLType::Local: {
    QString result = prefix + QStringLiteral("local:") + name;
    if (m_url.mask() == OmniPainter::CircleMask)
      result += QStringLiteral("?mask=circle");
    return result;
  }

  case ImageURLType::Http:
  case ImageURLType::Https:
    return prefix + QStringLiteral("http:") + name;

  case ImageURLType::Emoji:
    return prefix + QStringLiteral("emoji:") + name;

  case ImageURLType::Favicon:
    return prefix + QStringLiteral("favicon:") + name;

  case ImageURLType::DataURI:
    return prefix + QStringLiteral("datauri:") + name;

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
