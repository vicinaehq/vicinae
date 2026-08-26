#pragma once
#include <QString>
#include <cstdint>
#include <glaze/glaze.hpp>
#include <qjsonobject.h>

class CommandContext;

using LaunchContext = glz::generic;

struct LaunchProps {
  QString query;
  std::vector<std::pair<QString, QString>> arguments;
  std::optional<LaunchContext> launchContext;
  std::optional<QString> fallbackText;
  std::optional<QString> cwd;
};

enum CommandMode : std::uint8_t {
  CommandModeInvalid,
  CommandModeView,
  CommandModeNoView,
  CommandModeMenuBar
};
enum CommandType : std::uint8_t { CommandTypeBuiltin, CommandTypeExtension };
