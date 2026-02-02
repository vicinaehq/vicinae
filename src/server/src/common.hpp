#pragma once
#include <QString>
#include <cstdint>

class CommandContext;

struct LaunchProps {
  QString query;
  std::vector<std::pair<QString, QString>> arguments;
};

enum CommandMode : std::uint8_t {
  CommandModeInvalid,
  CommandModeView,
  CommandModeNoView,
  CommandModeMenuBar
};
enum CommandType : std::uint8_t { CommandTypeBuiltin, CommandTypeExtension };
