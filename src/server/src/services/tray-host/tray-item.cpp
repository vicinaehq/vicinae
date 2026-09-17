#include "tray-item.hpp"
#include <QDir>
#include <QDirIterator>
#include <QBuffer>

static QString findIconInThemePath(const QString &dir, const QString &name) {
  if (dir.isEmpty() || name.isEmpty() || !QDir(dir).exists()) return {};

  static const QStringList exts = {".svg", ".png", ".xpm"};
  QDirIterator it(dir, QDir::Files, QDirIterator::Subdirectories);
  QString best;
  qint64 bestSize = 0;

  while (it.hasNext()) {
    it.next();
    const auto info = it.fileInfo();
    if (info.completeBaseName() != name && info.baseName() != name) continue;
    const auto suffix = "." + info.suffix().toLower();
    if (!exts.contains(suffix)) continue;
    if (suffix == ".svg") return info.absoluteFilePath();
    if (info.size() > bestSize) {
      best = info.absoluteFilePath();
      bestSize = info.size();
    }
  }

  return best;
}

void TrayItem::resolveThemeIcons() {
  auto resolve = [this](const QString &name) {
    if (QDir::isAbsolutePath(name)) return name;
    return findIconInThemePath(iconThemePath, name);
  };
  iconPath = resolve(iconName);
  attentionIconPath = resolve(attentionIconName);
}

ImageURL TrayItem::icon() const {
  const bool attention = status == Status::NeedsAttention;
  const QString &name = attention && !attentionIconName.isEmpty() ? attentionIconName : iconName;
  const QString &path = attention && !attentionIconPath.isEmpty() ? attentionIconPath : iconPath;
  const QImage &pixmap = attention && !attentionIconPixmap.isNull() ? attentionIconPixmap : iconPixmap;

  if (!path.isEmpty()) return ImageURL::local(path);

  if (!pixmap.isNull()) {
    QByteArray bytes;
    QBuffer buf(&bytes);
    buf.open(QIODevice::WriteOnly);
    pixmap.save(&buf, "PNG");
    return ImageURL::rawData(bytes, "image/png");
  }

  if (!name.isEmpty()) return ImageURL::system(name);

  return ImageURL::system("application-x-executable");
}

QString TrayMenuItem::plainLabel() const {
  QString out;
  out.reserve(label.size());
  for (int i = 0; i < label.size(); ++i) {
    if (label[i] == '_') {
      if (i + 1 < label.size() && label[i + 1] == '_') {
        out += '_';
        ++i;
      }
      continue;
    }
    out += label[i];
  }
  return out;
}
