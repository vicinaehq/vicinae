#include "view-utils.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include "utils/utils.hpp"
#include <QFile>
#include <qcontainerfwd.h>
#include <cstddef>
#include <ranges>

namespace qml {

FilePreviewContent resolveFilePreview(const std::filesystem::path &path, QMimeDatabase &mimeDb) {
  FilePreviewContent result;
  auto qpath = QString::fromStdString(path.string());
  auto mime = mimeDb.mimeTypeForFile(qpath);
  result.mimeType = mime.name();

  if (mime.name().startsWith("image/")) {
    result.imageSource = imageSourceFor(ImageURL::local(path));
  } else if (Utils::isTextMimeType(mime)) {
    QFile file(qpath);
    if (file.open(QIODevice::ReadOnly) && file.size() <= MAX_PREVIEW_SIZE) {
      static constexpr qint64 MAX_DISPLAY = 10 * 1024;
      result.textContent = QString::fromUtf8(file.read(MAX_DISPLAY));
    }
  } else {
    result.imageSource = imageSourceFor(ImageURL::fileIcon(path));
  }

  return result;
}

QVariantList metadataToVariantList(const MetadataModel &metadata) {
  QVariantList result;
  for (const auto &child : metadata.children) {
    if (auto *label = std::get_if<MetadataLabel>(&child)) {
      QVariantMap entry;
      entry[QStringLiteral("type")] = QStringLiteral("label");
      entry[QStringLiteral("label")] = QString::fromStdString(label->title);
      entry[QStringLiteral("value")] = QString::fromStdString(label->text);
      if (label->icon) entry[QStringLiteral("icon")] = imageSourceFor(ImageURL(*label->icon));
      if (label->color) entry[QStringLiteral("valueColor")] = OmniPainter::resolveColor(*label->color).name();
      result.append(entry);
    } else if (auto *link = std::get_if<MetadataLink>(&child)) {
      QVariantMap entry;
      entry[QStringLiteral("type")] = QStringLiteral("link");
      entry[QStringLiteral("label")] = QString::fromStdString(link->title);
      entry[QStringLiteral("value")] = QString::fromStdString(link->text);
      entry[QStringLiteral("url")] = QString::fromStdString(link->target);
      result.append(entry);
    } else if (std::get_if<MetadataSeparator>(&child)) {
      QVariantMap entry;
      entry[QStringLiteral("type")] = QStringLiteral("separator");
      result.append(entry);
    } else if (auto *tags = std::get_if<TagListModel>(&child)) {
      QVariantMap entry;
      entry[QStringLiteral("type")] = QStringLiteral("tags");
      entry[QStringLiteral("label")] = QString::fromStdString(tags->title);
      QVariantList tagList;
      for (const auto &tag : tags->items) {
        QVariantMap t;
        t[QStringLiteral("text")] = QString::fromStdString(tag.text);
        if (tag.color) t[QStringLiteral("color")] = OmniPainter::resolveColor(*tag.color).name();
        if (tag.icon) t[QStringLiteral("icon")] = imageSourceFor(ImageURL(*tag.icon));
        tagList.append(t);
      }
      entry[QStringLiteral("tags")] = tagList;
      result.append(entry);
    }
  }
  return result;
}

QVariantMap accessoryToVariant(const ListAccessory &acc) {
  QVariantMap m;

  m[QStringLiteral("text")] = acc.text;
  if (acc.color) m[QStringLiteral("color")] = OmniPainter::resolveColor(*acc.color).name();
  m[QStringLiteral("fill")] = acc.fillBackground;
  if (acc.icon) {
    auto url = *acc.icon;
    if (acc.color && url.type() == ImageURLType::Builtin) url.setFill(acc.color);
    m[QStringLiteral("icon")] = imageSourceFor(url);
  }
  if (!acc.tooltip.isEmpty()) m[QStringLiteral("tooltip")] = acc.tooltip;

  return m;
}

QVariantList accessoriesToVariantList(const AccessoryList &accessories) {
  return accessories | std::views::transform(accessoryToVariant) | std::ranges::to<QVariantList>();
}

} // namespace qml
