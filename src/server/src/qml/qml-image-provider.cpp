#include "qml-image-provider.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include "ui/image/builtin-icon-loader.hpp"
#include "ui/image/emoji-image-loader.hpp"
#include "ui/image/local-image-loader.hpp"
#include "ui/image/qicon-image-loader.hpp"
#include <QCoreApplication>
#include <QEventLoop>
#include <QGuiApplication>
#include <QPixmapCache>
#include <QtMath>
#include <QThread>
#include <QTimer>

QmlImageProvider::QmlImageProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap) {}

// Synchronously render via a loader that communicates results through signals.
// Must be called on the main/GUI thread.
static QPixmap renderViaSignal(AbstractImageLoader &loader, const QSize &targetSize, qreal dpr = 1.0) {
  QEventLoop loop;
  QPixmap captured;
  bool gotResult = false;

  QObject::connect(&loader, &AbstractImageLoader::dataUpdated,
                   [&](const QPixmap &px, bool) {
                     captured = px;
                     gotResult = true;
                     loop.quit();
                   });
  QObject::connect(&loader, &AbstractImageLoader::errorOccured,
                   [&](const QString &) {
                     loop.quit();
                   });

  loader.render({.size = targetSize, .devicePixelRatio = dpr});

  if (!gotResult) {
    QTimer::singleShot(100, &loop, &QEventLoop::quit);
    loop.exec();
  }

  return captured;
}

// Core rendering logic. Must run on the main/GUI thread because
// QPainter on QPixmap, QPixmapCache, and QObject-based loaders are not thread-safe.
static QPixmap doRequestPixmap(const QString &id, QSize *size, const QSize &requestedSize) {
  int colonIdx = id.indexOf(':');
  if (colonIdx < 0) {
    if (size) *size = QSize(0, 0);
    return {};
  }

  QString type = id.left(colonIdx);
  QString name = id.mid(colonIdx + 1);

  qreal dpr = qGuiApp->devicePixelRatio();
  QSize logical = requestedSize.isValid() ? requestedSize : QSize(32, 32);
  QSize targetSize(qCeil(logical.width() * dpr), qCeil(logical.height() * dpr));

  // Check pixmap cache first
  QString cacheKey = QString("qml_%1_%2_%3x%4").arg(type, name).arg(targetSize.width()).arg(targetSize.height());
  QPixmap cached;
  if (QPixmapCache::find(cacheKey, &cached)) {
    if (size) *size = cached.size();
    return cached;
  }

  QPixmap result;

  // Resolve theme foreground color eagerly so SVG icons get a concrete fill
  QColor fgColor = ThemeService::instance().theme().resolve(SemanticColor::Foreground);

  if (type == "builtin") {
    // Parse optional tint parameters from name: "iconname?bg=N&fg=N"
    QString iconName = name;
    std::optional<SemanticColor> bgTint;
    std::optional<SemanticColor> fgTint;

    int qmark = name.indexOf('?');
    if (qmark >= 0) {
      iconName = name.left(qmark);
      QString params = name.mid(qmark + 1);
      for (const auto &param : params.split('&')) {
        int eq = param.indexOf('=');
        if (eq < 0) continue;
        QString key = param.left(eq);
        int val = param.mid(eq + 1).toInt();
        if (key == "bg") bgTint = static_cast<SemanticColor>(val);
        else if (key == "fg") fgTint = static_cast<SemanticColor>(val);
      }
    }

    QString iconPath = QString(":icons/%1.svg").arg(iconName);
    BuiltinIconLoader loader(iconPath);
    if (bgTint)
      loader.setBackgroundColor(*bgTint);
    if (fgTint)
      loader.setFillColor(*fgTint);
    result = loader.renderSync({.size = targetSize, .devicePixelRatio = dpr, .fill = fgColor});
  } else if (type == "system") {
    QIconImageLoader loader(name);
    result = renderViaSignal(loader, targetSize, dpr);
  } else if (type == "local") {
    std::filesystem::path path = name.toStdString();
    LocalImageLoader loader(path);
    result = renderViaSignal(loader, targetSize, dpr);
  } else if (type == "emoji") {
    EmojiImageLoader loader(name);
    result = renderViaSignal(loader, targetSize, dpr);
  }

  // Fallback: return a placeholder for unknown/unsupported types
  if (result.isNull()) {
    QString fallbackIcon = QString(":icons/question-mark-circle.svg");
    BuiltinIconLoader loader(fallbackIcon);
    result = loader.renderSync({.size = targetSize, .devicePixelRatio = dpr, .fill = fgColor});
  }

  if (!result.isNull()) {
    result.setDevicePixelRatio(dpr);
    QPixmapCache::insert(cacheKey, result);
  }

  if (size) *size = result.size();
  return result;
}

QPixmap QmlImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) {
  // requestPixmap may be called from Qt Quick's image loading thread.
  // QPainter on QPixmap, QPixmapCache, and QObject-based loaders require the GUI thread.
  if (QThread::currentThread() == QCoreApplication::instance()->thread()) {
    return doRequestPixmap(id, size, requestedSize);
  }

  QPixmap result;
  QMetaObject::invokeMethod(
      QCoreApplication::instance(),
      [&]() { result = doRequestPixmap(id, size, requestedSize); },
      Qt::BlockingQueuedConnection);
  return result;
}
