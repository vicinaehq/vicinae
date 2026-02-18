#pragma once
#include "ui/image/url.hpp"
#include <QMetaType>
#include <QString>

class QmlImageUrl {
  Q_GADGET
  Q_PROPERTY(bool valid READ isValid)
  Q_PROPERTY(bool isThemeSensitive READ isThemeSensitive)

public:
  QmlImageUrl() = default;
  explicit QmlImageUrl(ImageURL url);

  Q_INVOKABLE QString toSource() const;
  Q_INVOKABLE QmlImageUrl withFallback(const QmlImageUrl &fb) const;
  Q_INVOKABLE QmlImageUrl withBackgroundTint(const QString &tint) const;
  Q_INVOKABLE QmlImageUrl withFillColor(const QColor &color) const;
  Q_INVOKABLE QmlImageUrl withCircleMask() const;

  const ImageURL &imageUrl() const;
  bool isValid() const;
  bool isThemeSensitive() const;
private:
  ImageURL m_url;
};

Q_DECLARE_METATYPE(QmlImageUrl)
