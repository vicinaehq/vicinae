#include <QByteArray>
#include <QFileInfo>
#include <QMimeData>
#include <format>
#include <utility>
#include "attachment-content.hpp"
#include "service-registry.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/toast/toast-service.hpp"

AttachmentContent::AttachmentContent(FileAttachment data, std::optional<QString> path)
    : m_data(std::make_shared<const FileAttachment>(std::move(data))), m_path(std::move(path)) {}

void AttachmentContent::copyFile() const {
  auto *services = ServiceRegistry::instance();
  if (!m_path || !QFileInfo::exists(*m_path)) {
    services->toastService()->failure(tr("The original file is no longer available."));
    return;
  }
  if (!services->clipman()->copyFile(m_path->toStdString()))
    services->toastService()->failure(tr("Could not copy the file."));
}

void AttachmentContent::copyContents() const {
  if (!m_data) return;
  auto *services = ServiceRegistry::instance();
  bool copied;
  if (const auto *image = std::get_if<FileAttachment::Image>(&m_data->contents)) {
    auto *mime = new QMimeData;
    mime->setData(QString::fromStdString(image->mimeType),
                  QByteArray::fromBase64(QByteArray::fromStdString(image->base64)));
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
  source.setName(QString::fromStdString(std::format("data:{};base64,{}", image.mimeType, image.base64)));
  return ImageUrl(std::move(source));
}
