#pragma once

#include <QUrl>

#include <filesystem>
#include <optional>
#include <variant>
#include <vector>

#include "services/clipboard/clipboard-server.hpp"

namespace Clipboard {

using NoData = std::monostate;

struct File {
  std::filesystem::path path;
};

struct Urls {
  std::vector<QUrl> values;
};

struct Text {
  QString text;
};

struct Html {
  QString html;
  std::optional<QString> text;
};

struct SelectionRecordHandle {
  QString id;
};

using Content = std::variant<NoData, File, Urls, Text, Html, SelectionRecordHandle, ClipboardSelection>;

struct ReadContent {
  QString text;
  std::optional<QString> html;
  std::vector<QUrl> urls;
};

} // namespace Clipboard
