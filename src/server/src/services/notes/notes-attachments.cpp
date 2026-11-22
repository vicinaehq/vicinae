#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QMimeData>
#include <QMimeDatabase>
#include <QRegularExpression>
#include <QSaveFile>
#include <QScopeGuard>
#include <QUrl>
#include <algorithm>
#include <array>
#include <cmark-gfm.h>
#include <memory>
#include <set>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include "notes-attachments.hpp"
#include "notes-store.hpp"

namespace Notes {
namespace {
void fail(const QString &message) { throw std::runtime_error(message.toStdString()); }

bool hasLocalFiles(const QMimeData *mime) {
  const auto urls = mime->urls();
  return !urls.empty() && std::ranges::all_of(urls, [](const QUrl &url) { return url.isLocalFile(); });
}

constexpr auto IMAGE_FORMATS = std::to_array<std::pair<const char *, const char *>>(
    {{"image/png", "png"}, {"image/jpeg", "jpg"}, {"image/gif", "gif"}, {"image/webp", "webp"}});

std::vector<QString> attachmentPaths(const std::string &markdown) {
  const std::unique_ptr<cmark_node, decltype(&cmark_node_free)> root(
      cmark_parse_document(markdown.data(), markdown.size(), CMARK_OPT_DEFAULT), cmark_node_free);
  const std::unique_ptr<cmark_iter, decltype(&cmark_iter_free)> iter(cmark_iter_new(root.get()),
                                                                     cmark_iter_free);
  std::vector<QString> paths;
  paths.reserve(8);
  std::set<QString> seen;
  while (cmark_iter_next(iter.get()) != CMARK_EVENT_DONE) {
    if (cmark_iter_get_event_type(iter.get()) != CMARK_EVENT_ENTER) continue;
    auto *node = cmark_iter_get_node(iter.get());
    const auto type = cmark_node_get_type(node);
    if (type != CMARK_NODE_IMAGE && type != CMARK_NODE_LINK) continue;
    const QUrl url(QString::fromUtf8(cmark_node_get_url(node)));
    if (!url.isRelative() || !url.host().isEmpty()) continue;
    const auto path = url.path();
    const auto parts = path.split(u'/');
    if (parts.size() != 2 || !parts.front().endsWith(QLatin1String(".assets")) || parts.back().isEmpty() ||
        parts.back() == u"." || parts.back() == u".." || path.contains(u'\\'))
      continue;
    if (seen.emplace(path).second) paths.emplace_back(path);
  }
  return paths;
}

void ensureDirectory(const QString &path) {
  if (QFileInfo(path).isSymLink()) fail(Store::tr("The attachment folder must not be a symbolic link."));
  if (!QDir().mkpath(path) || !QFileInfo(path).isWritable())
    fail(Store::tr("Could not create a writable attachment folder."));
}

QString safeName(const std::string &name) {
  auto result = QString::fromStdString(name);
  result.replace(QRegularExpression(QStringLiteral(R"([\x00-\x1f<>:"/\\|?*])")), QStringLiteral("-"));
  while (result.endsWith(u'.') || result.endsWith(u' '))
    result.chop(1);
  if (result.isEmpty()) result = QStringLiteral("attachment");
  const QRegularExpression reserved(QStringLiteral(R"(^(CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])($|\.))"),
                                    QRegularExpression::CaseInsensitiveOption);
  if (reserved.match(result).hasMatch()) result.prepend(u'_');
  return result;
}

QString reserveFile(const QString &directory, const QString &name) {
  const QFileInfo info(name);
  const auto extension = info.suffix().isEmpty() ? QString() : u'.' + info.suffix();
  auto stem = info.completeBaseName();
  while (stem.toUtf8().size() + extension.toUtf8().size() > 180 && !stem.isEmpty())
    stem.chop(1);
  for (int suffix = 1;; ++suffix) {
    const auto filename = stem + (suffix == 1 ? QString() : QStringLiteral(" (%1)").arg(suffix)) + extension;
    QFile file(QDir(directory).filePath(filename));
    if (file.open(QIODevice::WriteOnly | QIODevice::NewOnly)) return file.fileName();
    if (!file.exists()) fail(file.errorString());
  }
}

QString escapeLabel(QString label) {
  label.replace(u'\\', QStringLiteral("\\\\"));
  for (const auto marker : QStringLiteral("[]*_`"))
    label.replace(marker, QString(u'\\') + marker);
  return label;
}

QByteArray fileHash(const QString &path) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly)) fail(file.errorString());
  QCryptographicHash hash(QCryptographicHash::Sha256);
  if (!hash.addData(&file)) fail(file.errorString());
  return hash.result();
}
} // namespace

bool canImportAttachments(const QMimeData *mime) {
  if (!mime) return false;
  if (hasLocalFiles(mime)) return true;
  // Spreadsheets and rich text editors offer a rendering of the copied text as an image.
  if (mime->hasFormat(QStringLiteral("text/plain"))) return false;
  return mime->hasImage() || std::ranges::any_of(IMAGE_FORMATS, [mime](const auto &format) {
           return mime->hasFormat(QLatin1String(format.first));
         });
}

std::vector<Attachment> attachmentsFromMimeData(const QMimeData *mime) {
  if (!mime) return {};
  if (hasLocalFiles(mime)) {
    const auto urls = mime->urls();
    std::vector<Attachment> attachments;
    attachments.reserve(urls.size());
    for (const auto &url : urls) {
      const auto path = url.toLocalFile();
      attachments.emplace_back(QFileInfo(path).fileName().toStdString(), path.toStdString());
    }
    return attachments;
  }
  const auto name = QStringLiteral("screenshot-%1")
                        .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd-HHmmss")));
  for (const auto &[format, suffix] : IMAGE_FORMATS) {
    if (const auto bytes = mime->data(QLatin1String(format)); !bytes.isEmpty())
      return {{(name + u'.' + QLatin1String(suffix)).toStdString(), bytes}};
  }
  if (mime->hasImage())
    return {{(name + QStringLiteral(".png")).toStdString(), qvariant_cast<QImage>(mime->imageData())}};
  return {};
}

std::string importAttachments(const Note &note, std::span<const Attachment> attachments) {
  const QFileInfo noteFile(QString::fromStdString(note.path));
  const QDir parent = noteFile.dir();
  auto folder = noteFile.completeBaseName() + QStringLiteral(".assets");
  for (const auto &path : attachmentPaths(note.markdown)) {
    const auto candidate = path.section(u'/', 0, 0);
    if (QFileInfo(parent.filePath(candidate)).isDir()) {
      folder = candidate;
      break;
    }
  }
  const auto directory = parent.filePath(folder);
  ensureDirectory(directory);
  std::vector<QString> created;
  created.reserve(attachments.size());
  auto rollback = qScopeGuard([&] {
    for (const auto &path : created)
      QFile::remove(path);
  });
  QStringList links;
  for (const auto &attachment : attachments) {
    const auto path = reserveFile(directory, safeName(attachment.name));
    created.emplace_back(path);
    QSaveFile output(path);
    if (!output.open(QIODevice::WriteOnly)) fail(output.errorString());
    std::visit(
        [&](const auto &content) {
          using T = std::decay_t<decltype(content)>;
          if constexpr (std::is_same_v<T, QByteArray>) {
            if (output.write(content) != content.size()) fail(output.errorString());
          } else if constexpr (std::is_same_v<T, QImage>) {
            if (content.isNull() || !content.save(&output, "PNG"))
              fail(Store::tr("Could not save the image."));
          } else {
            const auto source = QString::fromStdString(content);
            if (!QFileInfo(source).isFile()) fail(Store::tr("Only files can be attached to a note."));
            QFile input(source);
            if (!input.open(QIODevice::ReadOnly)) fail(input.errorString());
            while (!input.atEnd()) {
              const auto chunk = input.read(1024 * 1024);
              if (input.error() != QFileDevice::NoError) fail(input.errorString());
              if (output.write(chunk) != chunk.size()) fail(output.errorString());
            }
          }
        },
        attachment.content);
    if (!output.commit()) fail(output.errorString());
    QUrl relative;
    relative.setPath(parent.relativeFilePath(path));
    const bool image = QMimeDatabase().mimeTypeForFile(path).name().startsWith(QLatin1String("image/"));
    const auto label = image && !std::holds_alternative<std::string>(attachment.content)
                           ? Store::tr("Image")
                           : QFileInfo(path).fileName();
    links.append(QStringLiteral("%1[%2](<%3>)")
                     .arg(image ? QStringLiteral("!") : QString(), escapeLabel(label),
                          relative.toString(QUrl::FullyEncoded)));
  }
  rollback.dismiss();
  return links.join(u'\n').toStdString();
}

void copyAttachments(const Note &note, const QString &directory) {
  if (note.path.empty()) return;
  const auto source = QFileInfo(QString::fromStdString(note.path)).dir();
  const QDir destination(directory);
  if (source.absolutePath() == destination.absolutePath()) return;
  std::vector<QString> created;
  const auto paths = attachmentPaths(note.markdown);
  created.reserve(paths.size());
  auto rollback = qScopeGuard([&] {
    for (const auto &path : created)
      QFile::remove(path);
  });
  for (const auto &relative : paths) {
    const auto from = source.filePath(relative);
    if (!QFileInfo(from).isFile()) continue;
    const auto to = destination.filePath(relative);
    ensureDirectory(QFileInfo(to).absolutePath());
    if (QFileInfo(to).isSymLink()) fail(Store::tr("An attachment with this name already exists."));
    if (QFileInfo::exists(to)) {
      if (fileHash(from) != fileHash(to)) fail(Store::tr("An attachment with this name already exists."));
    } else {
      if (!QFile::copy(from, to)) fail(Store::tr("Could not copy an attachment: %1").arg(relative));
      created.emplace_back(to);
    }
  }
  rollback.dismiss();
}
} // namespace Notes
