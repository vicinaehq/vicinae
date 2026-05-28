#include "image-url.hpp"
#include "ui/omni-painter/omni-painter.hpp"

ImageUrl::ImageUrl(ImageURL url) : m_url(std::move(url)) {}

const ImageURL &ImageUrl::imageUrl() const { return m_url; }

bool ImageUrl::isValid() const { return m_url.type() != ImageURLType::Invalid; }

bool ImageUrl::isThemeSensitive() const {
  auto const type = m_url.type();
  if (type == ImageURLType::Builtin) return true;
  // Local may have a @light/@dark sibling on disk; we can't tell without stat'ing, so assume yes.
  if (type == ImageURLType::Local) return true;
  return m_url.fillColor().has_value() || m_url.backgroundTint().has_value();
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
