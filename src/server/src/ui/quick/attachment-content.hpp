#pragma once

#include <QCoreApplication>
#include <QMetaType>
#include <QString>
#include <QtQml/qqmlregistration.h>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include "ui/image/image-url.hpp"

struct FileAttachment {
  struct Image {
    std::string mimeType;
    std::string base64;
    std::size_t byteSize;
  };
  std::string name;
  std::variant<std::string, Image> contents;
  ImageUrl preview;

  bool isImage() const { return std::holds_alternative<Image>(contents); }
  std::size_t byteSize() const {
    if (const auto *image = std::get_if<Image>(&contents)) return image->byteSize;
    return std::get<std::string>(contents).size();
  }
};

class AttachmentContent {
  Q_GADGET
  QML_VALUE_TYPE(attachmentContent)
  Q_DECLARE_TR_FUNCTIONS(AttachmentContent)
  Q_PROPERTY(bool valid READ isValid)
  Q_PROPERTY(bool image READ isImage)
  Q_PROPERTY(bool hasFile READ hasFile)

public:
  Q_INVOKABLE void copyFile() const;
  Q_INVOKABLE void copyContents() const;
  Q_INVOKABLE ImageUrl imageSource() const;

  AttachmentContent() = default;
  explicit AttachmentContent(FileAttachment data, std::optional<QString> path);
  bool isValid() const { return bool(m_data); }
  bool isImage() const { return m_data && m_data->isImage(); }
  bool hasFile() const { return m_path.has_value(); }
  const FileAttachment &data() const { return *m_data; }

private:
  std::shared_ptr<const FileAttachment> m_data;
  std::optional<QString> m_path;
};

Q_DECLARE_METATYPE(AttachmentContent)
