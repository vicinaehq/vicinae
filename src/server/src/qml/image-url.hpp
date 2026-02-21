#pragma once
#include "ui/image/url.hpp"
#include <QMetaType>
#include <QString>

class ImageUrl {
  Q_GADGET
  Q_PROPERTY(bool valid READ isValid)
  Q_PROPERTY(bool isThemeSensitive READ isThemeSensitive)

public:
  ImageUrl() = default;
  explicit ImageUrl(ImageURL url);

  Q_INVOKABLE QString toSource() const;
  Q_INVOKABLE ImageUrl withFallback(const ImageUrl &fb) const;
  Q_INVOKABLE ImageUrl withBackgroundTint(const QString &tint) const;
  Q_INVOKABLE ImageUrl withFillColor(const QColor &color) const;
  Q_INVOKABLE ImageUrl withCircleMask() const;

  const ImageURL &imageUrl() const;
  bool isValid() const;
  bool isThemeSensitive() const;
private:
  ImageURL m_url;
};

Q_DECLARE_METATYPE(ImageUrl)
