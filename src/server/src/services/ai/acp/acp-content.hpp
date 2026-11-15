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
  if (block.type == "resource_link" && block.uri) {
    const auto uri = QString::fromStdString(*block.uri);
    const QUrl url(uri);
    const auto path = url.isLocalFile() ? url.toLocalFile() : uri;
    const QFileInfo info(path);
    if (!info.isAbsolute() || !info.isFile()) return std::nullopt;
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
      const QFileInfo info(url.toLocalFile());
      QFile file(info.absoluteFilePath());
      if (info.isFile() && file.open(QIODevice::ReadOnly)) {
        if (!result.mimeType)
          result.mimeType =
              QMimeDatabase().mimeTypeForFile(info, QMimeDatabase::MatchContent).name().toStdString();
        if (file.size() <= MAX_RESOURCE_BYTES) {
          auto bytes = file.read(MAX_RESOURCE_BYTES + 1);
          if (file.error() == QFileDevice::NoError && bytes.size() <= MAX_RESOURCE_BYTES)
            result.data = std::move(bytes);
        }
      } else {
        result.status = ResourcePart::Status::Unavailable;
      }
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

// Tool content updates replace the current snapshot and can resend earlier media.
class ToolMedia {
public:
  void replace(std::span<const ToolContent> contents) {
    m_media.clear();
    m_media.reserve(contents.size());
    for (const auto &item : contents) {
      if (item.type == "content" && item.content && item.content->type != "text")
        m_media.emplace_back(mediaContent(*item.content));
      else if (item.type != "content" && item.type != "diff")
        m_media.emplace_back(ResourcePart{.status = ResourcePart::Status::Unsupported});
    }
  }

  std::vector<Media> takeNew() {
    std::vector<Media> media;
    media.reserve(m_media.size());
    m_published.reserve(m_published.size() + m_media.size());
    for (auto &item : m_media) {
      QCryptographicHash hash(QCryptographicHash::Sha256);
      if (const auto *image = std::get_if<ImagePart>(&item)) {
        hash.addData(image->base64);
      } else {
        const auto &resource = std::get<ResourcePart>(item);
        for (const auto &field : {resource.name, resource.mimeType, resource.uri}) {
          hash.addData(QByteArray::fromStdString(field.value_or("")));
          hash.addData(QByteArrayView("\0", 1));
        }
        if (resource.data) hash.addData(*resource.data);
      }
      const auto digest = hash.result();
      if (std::ranges::contains(m_published, digest)) continue;
      m_published.emplace_back(digest);
      media.emplace_back(std::move(item));
    }
    m_media.clear();
    return media;
  }

private:
  std::vector<Media> m_media;
  std::vector<QByteArray> m_published;
};

} // namespace AI::acp
