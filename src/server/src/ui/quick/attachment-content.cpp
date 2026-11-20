#include <QByteArray>
#include <QFileInfo>
#include <QMimeData>
#include <QDir>
#include <QSaveFile>
#include <format>
#include <utility>
#include "attachment-content.hpp"
#include "service-registry.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/toast/toast-service.hpp"
#include "services/app-service/app-service.hpp"

AttachmentContent::AttachmentContent(FileAttachment data, std::optional<QString> path)
    : m_data(std::make_shared<const FileAttachment>(std::move(data))), m_path(std::move(path)) {}

AttachmentContent::AttachmentContent(AI::ResourcePart resource, std::optional<QString> path)
    : m_resource(std::make_shared<const AI::ResourcePart>(std::move(resource))), m_path(std::move(path)) {}

std::optional<QString> AttachmentContent::localPath() const {
  if (m_path && (QFileInfo::exists(*m_path) || (m_resource && m_resource->data))) return m_path;
  if (m_resource && m_resource->uri) {
    const QUrl url(QString::fromStdString(*m_resource->uri));
    if (url.isLocalFile() && QFileInfo::exists(url.toLocalFile())) return url.toLocalFile();
  }
  return std::nullopt;
}

bool AttachmentContent::hasFile() const { return localPath().has_value(); }

QString AttachmentContent::revealLabel() const {
#ifdef Q_OS_MACOS
  return tr("Show in Finder");
#elif defined(Q_OS_WIN)
  return tr("Show in File Explorer");
#else
  return tr("Show in File Manager");
#endif
}

bool AttachmentContent::canOpen() const {
  if (hasFile()) return true;
  if (!m_resource || !m_resource->uri) return false;
  const QUrl url(QString::fromStdString(*m_resource->uri));
  return url.isValid() && (url.scheme() == QLatin1String("https") || url.scheme() == QLatin1String("http"));
}

bool AttachmentContent::prepareFile() const {
  if (!m_path || QFileInfo::exists(*m_path) || !m_resource || !m_resource->data) return hasFile();
  if (!QDir().mkpath(QFileInfo(*m_path).absolutePath())) return false;
  QSaveFile file(*m_path);
  return file.open(QIODevice::WriteOnly) && file.write(*m_resource->data) == m_resource->data->size() &&
         file.commit();
}

void AttachmentContent::open() const {
  auto *services = ServiceRegistry::instance();
  if (const auto path = localPath(); path && prepareFile()) {
    if (services->appDb()->openTarget(QUrl::fromLocalFile(*path))) return;
  } else if (canOpen() && m_resource && m_resource->uri) {
    if (services->appDb()->openTarget(QUrl(QString::fromStdString(*m_resource->uri)))) return;
  }
  services->toastService()->failure(tr("Could not open the attachment."));
}

void AttachmentContent::reveal() const {
  auto *services = ServiceRegistry::instance();
  if (const auto path = localPath();
      path && prepareFile() && services->appDb()->showInFileBrowser(path->toStdString(), true))
    return;
  services->toastService()->failure(tr("The file is no longer available."));
}

void AttachmentContent::copyLink() const {
  if (!hasLink()) return;
  auto *services = ServiceRegistry::instance();
  if (!services->clipman()->copyText(QString::fromStdString(*m_resource->uri), {.concealed = false}))
    services->toastService()->failure(tr("Could not copy the link."));
}

void AttachmentContent::copyFile() const {
  auto *services = ServiceRegistry::instance();
  const auto path = localPath();
  if (!path || !prepareFile()) {
    services->toastService()->failure(tr("The original file is no longer available."));
    return;
  }
  if (!services->clipman()->copyFile(path->toStdString()))
    services->toastService()->failure(tr("Could not copy the file."));
}

void AttachmentContent::copyContents() const {
  if (!m_data) return;
  auto *services = ServiceRegistry::instance();
  bool copied;
  if (const auto *image = std::get_if<FileAttachment::Image>(&m_data->contents)) {
    auto *mime = new QMimeData;
    mime->setData(QString::fromStdString(image->mimeType), QByteArray::fromBase64(image->base64));
    copied = services->clipman()->copyQMimeData(mime, {.concealed = false});
  } else {
    copied = services->clipman()->copyText(QString::fromStdString(std::get<std::string>(m_data->contents)),
                                           {.concealed = false});
  }
  if (!copied) services->toastService()->failure(tr("Could not copy the attachment contents."));
}

ImageUrl AttachmentContent::imageSource() const {
  if (!isImage()) return {};
  const auto &image = std::get<FileAttachment::Image>(m_data->contents);
  ImageURL source;
  source.setType(ImageURLType::DataURI);
  source.setName(
      QString::fromStdString(std::format("data:{};base64,{}", image.mimeType, image.base64.toStdString())));
  return ImageUrl(std::move(source));
}
