#pragma once

#include <QCoreApplication>
#include <QMetaType>
#include <QString>
#include <QtQml/qqmlregistration.h>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include "common/file-attachment.hpp"

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
