#pragma once
#include <optional>
#include <string>
#include <QByteArray>
#include <QString>
#include "services/ai/text-rewrite.hpp"

/**
 * The title and keywords a vision model gives to a copied image, so that it can be found by what it shows.
 */
namespace ClipboardImageLabels {

struct Labels {
  QString title;
  QString keywords;
};

/**
 * The request asking the model to label the image, or nothing if the image can't be read. The image is
 * decoded and scaled down here, which is slow enough to be kept off the main thread.
 */
std::optional<TextRewrite::Request> request(std::string model, const QByteArray &image);

std::optional<Labels> parse(const QString &response);

} // namespace ClipboardImageLabels
