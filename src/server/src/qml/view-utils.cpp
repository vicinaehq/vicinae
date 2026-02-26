#include "view-utils.hpp"
#include "extend/accessory-model.hpp"
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
    if (file.open(QIODevice::ReadOnly)) {
      static constexpr qint64 MAX_PREVIEW = static_cast<qint64>(32 * 1024);
      result.textContent = QString::fromUtf8(file.read(MAX_PREVIEW));
    }
  } else {
    result.imageSource = imageSourceFor(
        ImageURL::system(mime.iconName()).withFallback(ImageURL::system(mime.genericIconName())));
  }

  return result;
}

QVariantList metadataToVariantList(const MetadataModel &metadata) {
  QVariantList result;
  for (const auto &child : metadata.children) {
    if (auto *label = std::get_if<MetadataLabel>(&child)) {
      QVariantMap entry;
      entry[QStringLiteral("type")] = QStringLiteral("label");
      entry[QStringLiteral("label")] = label->title;
      entry[QStringLiteral("value")] = label->text;
      if (label->icon) entry[QStringLiteral("icon")] = imageSourceFor(ImageURL(*label->icon));
      if (label->color) entry[QStringLiteral("valueColor")] = OmniPainter::resolveColor(*label->color).name();
      result.append(entry);
    } else if (auto *link = std::get_if<MetadataLink>(&child)) {
      QVariantMap entry;
      entry[QStringLiteral("type")] = QStringLiteral("link");
      entry[QStringLiteral("label")] = link->title;
      entry[QStringLiteral("value")] = link->text;
      entry[QStringLiteral("url")] = link->target;
      result.append(entry);
    } else if (std::get_if<MetadataSeparator>(&child)) {
      QVariantMap entry;
      entry[QStringLiteral("type")] = QStringLiteral("separator");
      result.append(entry);
    } else if (auto *tags = std::get_if<TagListModel>(&child)) {
      QVariantMap entry;
      entry[QStringLiteral("type")] = QStringLiteral("tags");
      entry[QStringLiteral("label")] = tags->title;
      QVariantList tagList;
      for (const auto &tag : tags->items) {
        QVariantMap t;
        t[QStringLiteral("text")] = tag.text;
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

QVariantMap accessoryToVariant(const AccessoryModel &acc) {
  QVariantMap m;
  bool fill = false;

  if (auto *tag = std::get_if<AccessoryModel::Tag>(&acc.data)) {
    m[QStringLiteral("text")] = tag->value;
    if (tag->color) m[QStringLiteral("color")] = OmniPainter::resolveColor(*tag->color).name();
    fill = true;
  } else if (auto *text = std::get_if<AccessoryModel::Text>(&acc.data)) {
    m[QStringLiteral("text")] = text->value;
    if (text->color) m[QStringLiteral("color")] = OmniPainter::resolveColor(*text->color).name();
  }

  m[QStringLiteral("fill")] = fill;
  if (acc.icon) {
    auto url = ImageURL(*acc.icon);
    auto color = std::visit([](const auto &v) { return v.color; }, acc.data);
    if (color && url.type() == ImageURLType::Builtin) url.setFill(color);
    m[QStringLiteral("icon")] = imageSourceFor(url);
  }
  if (acc.tooltip) m[QStringLiteral("tooltip")] = *acc.tooltip;

  return m;
}

QVariantList accessoriesToVariantList(const std::vector<AccessoryModel> &accessories) {
  return accessories | std::views::transform(accessoryToVariant) | std::ranges::to<QVariantList>();
}

} // namespace qml
