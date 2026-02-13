#pragma once
#include "qml-image-url.hpp"
#include <QObject>

class QmlImageSource : public QObject {
  Q_OBJECT

public:
  explicit QmlImageSource(QObject *parent = nullptr) : QObject(parent) {}

  Q_INVOKABLE QmlImageUrl builtin(const QString &name) const {
    return QmlImageUrl(ImageURL::builtin(name));
  }

  Q_INVOKABLE QmlImageUrl system(const QString &name) const {
    return QmlImageUrl(ImageURL::system(name));
  }

  Q_INVOKABLE QmlImageUrl local(const QString &path) const {
    return QmlImageUrl(ImageURL::local(path));
  }

  Q_INVOKABLE QmlImageUrl http(const QString &url) const {
    return QmlImageUrl(ImageURL::http(QUrl(url)));
  }

  Q_INVOKABLE QmlImageUrl emoji(const QString &emoji) const {
    return QmlImageUrl(ImageURL::emoji(emoji));
  }

  Q_INVOKABLE QmlImageUrl favicon(const QString &domain) const {
    return QmlImageUrl(ImageURL::favicon(domain));
  }

  Q_INVOKABLE QmlImageUrl fileIcon(const QString &path) const {
    return QmlImageUrl(ImageURL::fileIcon(path.toStdString()));
  }

  Q_INVOKABLE QmlImageUrl parse(const QString &iconUrl) const {
    return QmlImageUrl(ImageURL(iconUrl));
  }
};
