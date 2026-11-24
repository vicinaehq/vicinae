#include "clipboard-image-labels.hpp"
#include <utility>
#include <QBuffer>
#include <QImage>
#include <QStringView>
#include "ui/image/image-renderer.hpp"

namespace ClipboardImageLabels {

namespace {

constexpr int MAX_IMAGE_EDGE = 1280;
constexpr qsizetype MAX_TITLE_LENGTH = 50;
constexpr qsizetype MAX_KEYWORDS_LENGTH = 200;

constexpr auto SYSTEM_PROMPT =
    "You label images saved in a clipboard history so that they can be found later with a text search.\n"
    "Reply with exactly two lines and nothing else:\n"
    "Title: a short descriptive title, at most 50 characters\n"
    "Keywords: 5 to 10 comma-separated lowercase keywords covering what the image shows and any prominent "
    "text visible in it";
constexpr auto PROMPT = "Label this image.";

std::optional<QString> fieldValue(QStringView line, QStringView field) {
  if (!line.startsWith(field, Qt::CaseInsensitive)) return std::nullopt;

  auto value = line.sliced(field.size()).toString().simplified();
  if (value.size() >= 2 && value.startsWith('"') && value.endsWith('"'))
    value = value.sliced(1, value.size() - 2);

  return value;
}

} // namespace

std::optional<TextRewrite::Request> request(std::string model, const QByteArray &image) {
  const auto decoded = ImageRendering::decodeImageData(image, {MAX_IMAGE_EDGE, MAX_IMAGE_EDGE});
  if (decoded.isNull()) return std::nullopt;

  QByteArray png;
  QBuffer output(&png);
  if (!decoded.save(&output, "PNG")) return std::nullopt;

  return TextRewrite::Request{.model = std::move(model),
                              .systemPrompt = SYSTEM_PROMPT,
                              .prompt = PROMPT,
                              .images = {AI::ImagePart{.mimeType = "image/png", .base64 = png.toBase64()}}};
}

std::optional<Labels> parse(const QString &response) {
  Labels labels;

  for (const auto rawLine : QStringView(response).split('\n', Qt::SkipEmptyParts)) {
    // small models like to answer with markdown emphasis around the field names
    const auto line = rawLine.toString().remove('*').trimmed();

    if (auto title = fieldValue(line, u"title:")) {
      labels.title = title->left(MAX_TITLE_LENGTH);
    } else if (auto keywords = fieldValue(line, u"keywords:")) {
      labels.keywords = keywords->left(MAX_KEYWORDS_LENGTH);
    }
  }

  if (labels.title.isEmpty() || labels.keywords.isEmpty()) return std::nullopt;

  return labels;
}

} // namespace ClipboardImageLabels
