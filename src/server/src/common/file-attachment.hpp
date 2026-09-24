#pragma once

#include <QByteArray>
#include <string>
#include <variant>
#include "ui/image/image-url.hpp"

struct FileAttachment {
  struct Image {
    std::string mimeType;
    QByteArray base64;
    std::size_t byteSize;
  };
  std::string name;
  std::variant<std::string, Image> contents;
  ImageUrl preview;
  QByteArray originalBytes;
  std::string originalMimeType;

  bool isImage() const { return std::holds_alternative<Image>(contents); }
  std::size_t byteSize() const {
    if (const auto *image = std::get_if<Image>(&contents)) return image->byteSize;
    return std::get<std::string>(contents).size();
  }
};
