#pragma once

#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QUrl>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <vector>
#include <variant>
#include "services/ai/chat-image.hpp"
#include "services/ai/chat-resource.hpp"

namespace AI::acp {

struct ResourceContent {
  std::optional<std::string> mimeType;
  std::optional<std::string> blob;
  std::optional<std::string> text;
  std::optional<std::string> uri;
};

struct ContentBlock {
  std::optional<std::string> type;
  std::optional<std::string> text;
  std::optional<std::string> data;
  std::optional<std::string> mimeType;
  std::optional<ResourceContent> resource;
  std::optional<std::string> uri;
  std::optional<std::string> name;
};

struct ToolContent {
  std::optional<std::string> type;
  std::optional<ContentBlock> content;
  std::optional<std::string> path;
  std::optional<std::string> newText;
};

inline std::optional<ImagePart> imageContent(const ContentBlock &block) {
  constexpr qint64 MAX_LINKED_IMAGE_BYTES = 20 * 1024 * 1024;
  if (block.type == "resource_link" && block.uri) {
    const auto uri = QString::fromStdString(*block.uri);
    const QUrl url(uri);
    const auto path = url.isLocalFile() ? url.toLocalFile() : uri;
    const QFileInfo info(path);
    if (!info.isAbsolute() || !info.isFile() || info.size() > MAX_LINKED_IMAGE_BYTES) return std::nullopt;
    const auto mime = QMimeDatabase().mimeTypeForFile(path, QMimeDatabase::MatchContent).name();
    if (!mime.startsWith(QLatin1String("image/"))) return std::nullopt;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return std::nullopt;
    const auto bytes = file.readAll();
    if (bytes.isEmpty()) return std::nullopt;
    return ImagePart{mime.toStdString(), bytes.toBase64()};
  }
  const auto *data = &block.data;
  const auto *mime = &block.mimeType;
  if (block.type == "resource" && block.resource) {
    data = &block.resource->blob;
    mime = &block.resource->mimeType;
  } else if (block.type != "image") {
    return std::nullopt;
  }
  if (!*data || !*mime || !(**mime).starts_with("image/")) return std::nullopt;
  auto decoded = QByteArray::fromBase64Encoding(QByteArray::fromStdString(**data),
                                                QByteArray::AbortOnBase64DecodingErrors);
  if (!decoded || decoded.decoded.isEmpty()) return std::nullopt;
  return ImagePart{**mime, decoded.decoded.toBase64()};
}

inline ResourcePart resourceContent(const ContentBlock &block) {
  constexpr qsizetype MAX_RESOURCE_BYTES = 50 * 1024 * 1024;
  ResourcePart result{.name = block.name, .mimeType = block.mimeType, .uri = block.uri};
  const auto *encoded = &block.data;
  if (block.type == "resource" && block.resource) {
    result.mimeType = block.resource->mimeType;
    result.uri = block.resource->uri;
    encoded = &block.resource->blob;
    if (block.resource->text) {
      result.data = QByteArray::fromStdString(*block.resource->text);
      if (!result.mimeType) result.mimeType = "text/plain";
    }
  }
  if (*encoded) {
    auto decoded = QByteArray::fromBase64Encoding(QByteArray::fromStdString(**encoded),
                                                  QByteArray::AbortOnBase64DecodingErrors);
    if (decoded && decoded.decoded.size() <= MAX_RESOURCE_BYTES)
      result.data = std::move(decoded.decoded);
    else
      result.status = ResourcePart::Status::Unavailable;
  }
  if (result.uri) {
    auto uri = QString::fromStdString(*result.uri);
    auto url = QUrl(uri);
    if (QFileInfo(uri).isAbsolute()) url = QUrl::fromLocalFile(uri);
    result.uri = url.toString().toStdString();
    if (!result.name && !url.fileName().isEmpty()) result.name = url.fileName().toStdString();
    if (url.isLocalFile() && !result.data) {
      if (!result.mimeType)
        result.mimeType = QMimeDatabase()
                              .mimeTypeForFile(url.toLocalFile(), QMimeDatabase::MatchExtension)
                              .name()
                              .toStdString();
    } else if (!url.isLocalFile() && url.scheme() != QLatin1String("https") &&
               url.scheme() != QLatin1String("http") && !result.data) {
      result.status = ResourcePart::Status::Unsupported;
    }
  }
  if (result.data && result.data->size() > MAX_RESOURCE_BYTES) {
    result.data.reset();
    result.status = ResourcePart::Status::Unavailable;
  }
  if (!result.data && !result.uri)
    result.status = block.type == "resource" || block.type == "image" || block.type == "audio"
                        ? ResourcePart::Status::Unavailable
                        : ResourcePart::Status::Unsupported;
  if (result.name) {
    auto name = QString::fromStdString(*result.name).replace(QLatin1Char('\\'), QLatin1Char('/'));
    name = QFileInfo(name).fileName();
    if (name.isEmpty() || name == QLatin1String(".") || name == QLatin1String(".."))
      result.name.reset();
    else
      result.name = name.toStdString();
  }
  if (!result.name && result.data) {
    const auto suffix = QMimeDatabase()
                            .mimeTypeForName(QString::fromStdString(result.mimeType.value_or("")))
                            .preferredSuffix();
    result.name = suffix.isEmpty() ? "attachment" : (QStringLiteral("attachment.") + suffix).toStdString();
  }
  return result;
}

using Media = std::variant<ImagePart, ResourcePart>;

inline Media mediaContent(const ContentBlock &block) {
  if (auto image = imageContent(block)) return std::move(*image);
  return resourceContent(block);
}

class ToolMedia {
public:
  void replace(std::span<const ToolContent> contents) {
    m_pending.clear();
    m_pending.reserve(contents.size());
    for (const auto &item : contents) {
      if (item.type == "content" && item.content && item.content->type != "text")
        m_pending.emplace_back(*item.content);
      else if (item.type != "content" && item.type != "diff")
        m_pending.emplace_back(std::nullopt);
    }
  }

  std::vector<Media> takeNew() {
    std::vector<Media> media;
    media.reserve(m_pending.size());
    m_published.reserve(m_published.size() + m_pending.size());
    for (const auto &block : m_pending) {
      auto digest = fingerprint(block);
      if (std::ranges::contains(m_published, digest)) continue;
      m_published.emplace_back(std::move(digest));
      if (block)
        media.emplace_back(mediaContent(*block));
      else
        media.emplace_back(ResourcePart{.status = ResourcePart::Status::Unsupported});
    }
    m_pending.clear();
    return media;
  }

private:
  static QByteArray fingerprint(const std::optional<ContentBlock> &block) {
    QCryptographicHash hash(QCryptographicHash::Sha256);
    const auto add = [&hash](const std::optional<std::string> &field) {
      hash.addData(QByteArrayView(field.value_or("")));
      hash.addData(QByteArrayView("\0", 1));
    };
    if (block) {
      for (const auto *field :
           {&block->type, &block->text, &block->data, &block->mimeType, &block->uri, &block->name})
        add(*field);
      if (const auto &resource = block->resource)
        for (const auto *field : {&resource->mimeType, &resource->blob, &resource->text, &resource->uri})
          add(*field);
    }
    return hash.result();
  }

  std::vector<std::optional<ContentBlock>> m_pending;
  std::vector<QByteArray> m_published;
};

} // namespace AI::acp
