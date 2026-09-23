#include <QBuffer>
#include <QClipboard>
#include <QFile>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QLocale>
#include <QMimeData>
#include <QMimeDatabase>
#include <QStringDecoder>
#include <QtConcurrentRun>
#include <algorithm>
#include <utility>
#include "attachment-model.hpp"
#include "ui/image/image-renderer.hpp"

namespace {
constexpr int MAX_ATTACHMENTS = 8;
constexpr qint64 MAX_FILE_BYTES = 50 * 1024 * 1024;
constexpr qint64 MAX_TEXT_BYTES = 128 * 1024;
constexpr int MAX_IMAGE_EDGE = 2048;
constexpr int PREVIEW_EDGE = 256;
} // namespace

AttachmentModel::AttachmentModel(QObject *parent) : QObject(parent) { m_entries.reserve(MAX_ATTACHMENTS); }

QVariantList AttachmentModel::items() const {
  QVariantList result;
  result.reserve(m_entries.size());
  for (const auto &entry : m_entries) {
    const auto preview = entry.attachment.isImage()
                             ? entry.attachment.data().preview
                             : ImageUrl(ImageURL::fileIcon(entry.path.value_or(entry.name).toStdString()));
    QVariantMap item{{QStringLiteral("id"), entry.id},
                     {QStringLiteral("name"), entry.name},
                     {QStringLiteral("loading"), entry.loading},
                     {QStringLiteral("error"), entry.error},
                     {QStringLiteral("preview"), QVariant::fromValue(preview)},
                     {QStringLiteral("content"), QVariant::fromValue(entry.attachment)}};
    if (entry.attachment.isValid()) {
      item.insert(QStringLiteral("detail"), QLocale().formattedDataSize(entry.attachment.data().byteSize()));
    }
    result.append(std::move(item));
  }
  return result;
}

bool AttachmentModel::ready() const {
  return std::ranges::all_of(m_entries, [](const Entry &entry) { return entry.attachment.isValid(); });
}

bool AttachmentModel::hasImages() const {
  return std::ranges::any_of(m_entries, [](const Entry &entry) { return entry.attachment.isImage(); });
}

bool AttachmentModel::canAdd() {
  m_error.clear();
  if (count() < MAX_ATTACHMENTS) return true;
  m_error = tr("You can attach up to %1 files per message.").arg(MAX_ATTACHMENTS);
  emit changed();
  return false;
}

void AttachmentModel::load(Entry entry, QFuture<Result> future) {
  const int id = entry.id;
  m_entries.emplace_back(std::move(entry));
  emit changed();
  auto *watcher = new QFutureWatcher<Result>(this);
  connect(watcher, &QFutureWatcher<Result>::finished, this, [this, watcher, id] {
    auto result = watcher->future().takeResult();
    watcher->deleteLater();
    const auto entry = std::ranges::find(m_entries, id, &Entry::id);
    if (entry == m_entries.end()) return;
    entry->loading = false;
    if (result && !result->isImage()) {
      auto total = result->byteSize();
      for (const auto &other : m_entries)
        if (other.attachment.isValid() && !other.attachment.isImage())
          total += other.attachment.data().byteSize();
      if (total > MAX_TEXT_BYTES)
        result = std::unexpected(
            tr("Text attachments must total less than %1 KB per message.").arg(MAX_TEXT_BYTES / 1024));
    }
    if (result)
      entry->attachment = AttachmentContent(std::move(*result), entry->path);
    else
      entry->error = std::move(result.error());
    emit changed();
  });
  watcher->setFuture(std::move(future));
}

void AttachmentModel::addFiles(const QStringList &paths) {
  for (const auto &path : paths) {
    const auto absolute = QFileInfo(path).absoluteFilePath();
    if (std::ranges::any_of(m_entries, [&](const Entry &entry) { return entry.path == absolute; })) continue;
    if (!canAdd()) break;
    load(Entry{.id = m_nextId++, .name = QFileInfo(path).fileName(), .path = absolute},
         QtConcurrent::run([absolute] { return readFile(absolute); }));
  }
}

bool AttachmentModel::acceptsUrls(const QList<QUrl> &urls) const {
  return !urls.empty() && std::ranges::all_of(urls, [](const QUrl &url) { return url.isLocalFile(); });
}

void AttachmentModel::addUrls(const QList<QUrl> &urls) {
  QStringList paths;
  paths.reserve(urls.size());
  for (const auto &url : urls)
    if (url.isLocalFile()) paths.append(url.toLocalFile());
  addFiles(paths);
}

bool AttachmentModel::paste() {
  const auto *mime = QGuiApplication::clipboard()->mimeData();
  if (!mime) return false;
  const auto urls = mime->urls();
  if (std::ranges::any_of(urls, [](const QUrl &url) { return url.isLocalFile(); })) {
    addUrls(urls);
    return true;
  }
  if (!mime->hasImage()) return false;
  if (!canAdd()) return true;
  const auto name = tr("Pasted image") + QStringLiteral(".png");
  const auto image = qvariant_cast<QImage>(mime->imageData());
  load(Entry{.id = m_nextId++, .name = name},
       QtConcurrent::run([image, name] { return prepareImage(image, name.toStdString()); }));
  return true;
}

void AttachmentModel::remove(int id) {
  std::erase_if(m_entries, [id](const Entry &entry) { return entry.id == id; });
  m_error.clear();
  emit changed();
}

void AttachmentModel::clear() {
  m_entries.clear();
  m_error.clear();
  emit changed();
}

std::vector<AttachmentContent> AttachmentModel::take() {
  if (!ready()) return {};
  std::vector<AttachmentContent> result;
  result.reserve(m_entries.size());
  for (auto &entry : m_entries)
    result.emplace_back(std::move(entry.attachment));
  clear();
  return result;
}

AttachmentModel::Result AttachmentModel::prepareImage(QImage image, std::string name) {
  if (image.isNull()) return std::unexpected(tr("This image could not be read."));
  if (image.width() > MAX_IMAGE_EDGE || image.height() > MAX_IMAGE_EDGE)
    image = image.scaled(MAX_IMAGE_EDGE, MAX_IMAGE_EDGE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  QByteArray data;
  QBuffer output(&data);
  if (!image.save(&output, "PNG")) return std::unexpected(tr("This image could not be prepared."));
  QByteArray preview;
  QBuffer thumbnail(&preview);
  image.scaled(PREVIEW_EDGE, PREVIEW_EDGE, Qt::KeepAspectRatio, Qt::SmoothTransformation)
      .save(&thumbnail, "PNG");
  return FileAttachment{std::move(name),
                        FileAttachment::Image{"image/png", data.toBase64().toStdString(),
                                              static_cast<std::size_t>(data.size())},
                        ImageUrl(ImageURL::rawData(preview, QStringLiteral("image/png")))};
}

AttachmentModel::Result AttachmentModel::readFile(const QString &path) {
  const QFileInfo info(path);
  if (!info.isFile()) return std::unexpected(tr("Choose an image or a text file."));
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly)) return std::unexpected(tr("This file could not be opened."));
  if (file.size() > MAX_FILE_BYTES)
    return std::unexpected(tr("Files must be smaller than %1 MB.").arg(MAX_FILE_BYTES / (1024 * 1024)));
  const auto data = file.read(MAX_FILE_BYTES + 1);
  if (file.error() != QFileDevice::NoError) return std::unexpected(tr("This file could not be read."));
  if (data.size() > MAX_FILE_BYTES) return std::unexpected(tr("This file is too large."));
  const auto mime = QMimeDatabase().mimeTypeForFileNameAndData(info.fileName(), data);
  if (mime.name().startsWith("image/"))
    return prepareImage(ImageRendering::decodeImageData(data, {MAX_IMAGE_EDGE, MAX_IMAGE_EDGE}),
                        info.fileName().toStdString());
  if (!mime.inherits("text/plain") && !mime.inherits("application/json") &&
      !mime.inherits("application/xml") && mime.name() != "application/javascript" &&
      mime.name() != "application/x-empty" && mime.name() != "application/octet-stream")
    return std::unexpected(tr("Only images and UTF-8 text files are supported."));
  if (data.size() > MAX_TEXT_BYTES)
    return std::unexpected(tr("Text files must be smaller than %1 KB.").arg(MAX_TEXT_BYTES / 1024));
  QStringDecoder decoder(QStringDecoder::Utf8, QStringConverter::Flag::Stateless);
  const QString text = decoder(data);
  if (decoder.hasError() || std::ranges::any_of(text, [](QChar ch) {
        return ch.category() == QChar::Other_Control && ch != '\n' && ch != '\r' && ch != '\t';
      }))
    return std::unexpected(tr("Only images and UTF-8 text files are supported."));
  return FileAttachment{info.fileName().toStdString(), text.toStdString(), {}};
}

void AttachmentPasteHandler::setTarget(QQuickItem *target) {
  if (m_target == target) return;
  if (m_target) m_target->removeEventFilter(this);
  m_target = target;
  if (m_target) m_target->installEventFilter(this);
  emit targetChanged();
}

bool AttachmentPasteHandler::eventFilter(QObject *object, QEvent *event) {
  if (object != m_target || !m_enabled || !m_model || event->type() != QEvent::KeyPress) return false;
  // NOLINTBEGIN(cppcoreguidelines-pro-type-static-cast-downcast)
  return static_cast<QKeyEvent *>(event)->matches(QKeySequence::Paste) && m_model->paste();
  // NOLINTEND(cppcoreguidelines-pro-type-static-cast-downcast)
}
