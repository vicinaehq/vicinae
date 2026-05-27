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

static void resolveColors(ImageURL &url) {
  if (auto fill = url.fillColor())
    url.setFill(OmniPainter::resolveColor(*fill));
  else if (url.type() == ImageURLType::Builtin)
    url.setFill(ThemeService::instance().theme().resolve(SemanticColor::Foreground));

  if (auto bg = url.backgroundTint()) url.setBackgroundTint(OmniPainter::resolveColor(*bg));
}

QString ImageUrl::toSource() const {
  if (!isValid()) return {};

  ImageURL resolved = m_url;
  resolveColors(resolved);

  if (auto fb = m_url.fallback()) {
    ImageURL fbUrl(*fb);
    resolveColors(fbUrl);
    resolved.withFallback(fbUrl);
  }

  return QStringLiteral("image://vicinae/") + resolved.toString();
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
