#pragma once

#include <QByteArray>
#include <QFuture>
#include <QString>
#include <expected>
#include <string>
#include <vector>

namespace AppleShortcuts {

struct Shortcut {
  std::string id;
  std::string name;
  QByteArray icon;

  bool operator==(const Shortcut &) const = default;
};

using ListResult = std::expected<std::vector<Shortcut>, QString>;
using RunResult = std::expected<void, QString>;

QFuture<ListResult> list();
// Reports dispatch errors only; Shortcuts Events handles execution independently.
QFuture<RunResult> run(std::string id);

} // namespace AppleShortcuts
