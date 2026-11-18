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
#include "services/ai/chat-resource.hpp"

class AttachmentContent {
  Q_GADGET
  QML_VALUE_TYPE(attachmentContent)
  Q_DECLARE_TR_FUNCTIONS(AttachmentContent)
  Q_PROPERTY(bool valid READ isValid)
  Q_PROPERTY(bool image READ isImage)
  Q_PROPERTY(bool hasFile READ hasFile)
  Q_PROPERTY(bool canOpen READ canOpen)
  Q_PROPERTY(bool canCopyContents READ canCopyContents)
  Q_PROPERTY(bool hasLink READ hasLink)
  Q_PROPERTY(QString revealLabel READ revealLabel)

public:
  Q_INVOKABLE void copyFile() const;
  Q_INVOKABLE void copyContents() const;
  Q_INVOKABLE ImageUrl imageSource() const;
  Q_INVOKABLE void open() const;
  Q_INVOKABLE void reveal() const;
  Q_INVOKABLE void copyLink() const;

  AttachmentContent() = default;
  explicit AttachmentContent(FileAttachment data, std::optional<QString> path);
  explicit AttachmentContent(AI::ResourcePart resource, std::optional<QString> path);
  bool isValid() const { return bool(m_data) || bool(m_resource); }
  bool isImage() const { return m_data && m_data->isImage(); }
  bool hasFile() const;
  bool canOpen() const;
  bool canCopyContents() const { return bool(m_data); }
  bool hasLink() const { return m_resource && m_resource->uri.has_value(); }
  QString revealLabel() const;
  const FileAttachment &data() const { return *m_data; }

private:
  std::optional<QString> localPath() const;
  bool prepareFile() const;
  std::shared_ptr<const FileAttachment> m_data;
  std::shared_ptr<const AI::ResourcePart> m_resource;
  std::optional<QString> m_path;
};

Q_DECLARE_METATYPE(AttachmentContent)
