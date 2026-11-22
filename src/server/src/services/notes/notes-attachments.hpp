#pragma once

#include <QByteArray>
#include <QImage>
#include <span>
#include <string>
#include <variant>
#include <vector>

class QMimeData;

namespace Notes {
struct Note;

struct Attachment {
  std::string name;
  std::variant<QByteArray, QImage, std::string> content;
};

bool canImportAttachments(const QMimeData *mime);
std::vector<Attachment> attachmentsFromMimeData(const QMimeData *mime);
std::string importAttachments(const Note &note, std::span<const Attachment> attachments);
void copyAttachments(const Note &note, const QString &directory);
} // namespace Notes
