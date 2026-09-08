#pragma once
#include "ui/qml-engine-scope.hpp"
#include "ui/image/image-url.hpp"
#include <QObject>

class ImageSource : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(Img)
  QML_SINGLETON

public:
  static ImageSource *create(QQmlEngine *, QJSEngine *) { return QmlEngineScope::global<ImageSource>(); }

private:
public:
  explicit ImageSource(QObject *parent) : QObject(parent) {}

  Q_INVOKABLE ImageUrl builtin(const QString &name) const { return ImageUrl(ImageURL::builtinByName(name)); }

  Q_INVOKABLE ImageUrl system(const QString &name) const { return ImageUrl(ImageURL::system(name)); }

  Q_INVOKABLE ImageUrl local(const QString &path) const { return ImageUrl(ImageURL::local(path)); }

  Q_INVOKABLE ImageUrl http(const QString &url) const { return ImageUrl(ImageURL::http(QUrl(url))); }

  Q_INVOKABLE ImageUrl emoji(const QString &emoji) const { return ImageUrl(ImageURL::emoji(emoji)); }

  Q_INVOKABLE ImageUrl favicon(const QString &domain) const { return ImageUrl(ImageURL::favicon(domain)); }

  Q_INVOKABLE ImageUrl fileIcon(const QString &path) const {
    return ImageUrl(ImageURL::fileIcon(path.toStdString()));
  }

  Q_INVOKABLE ImageUrl parse(const QString &iconUrl) const { return ImageUrl(ImageURL(iconUrl)); }
};
