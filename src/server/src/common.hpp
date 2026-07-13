#pragma once
#include <QString>
#include <cstdint>
#include <optional>
#include <qjsonobject.h>

class CommandContext;

using LaunchContext = QJsonObject;

struct LaunchProps {
  QString query;
  std::vector<std::pair<QString, QString>> arguments;
  std::optional<LaunchContext> launchContext;
};

enum CommandMode : std::uint8_t {
  CommandModeInvalid,
  CommandModeView,
  CommandModeNoView,
  CommandModeMenuBar
};
enum CommandType : std::uint8_t { CommandTypeBuiltin, CommandTypeExtension };
