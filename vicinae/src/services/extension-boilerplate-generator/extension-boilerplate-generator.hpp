#pragma once
#include "common.hpp"
#include <expected>
#include <qstring.h>
#include <filesystem>

struct ExtensionBoilerplateConfig {
  struct CommandConfig {
    QString title;
    QString subtitle;
    QString description;
    QString templateId;
  };

  QString author;
  QString title;
  QString description;
  std::vector<CommandConfig> commands;
};

struct CommandBoilerplate {
  QString resource;
  QString name;
  CommandMode mode;
};

class ExtensionBoilerplateGenerator {
  // returns the generated extension path or an error
  using BoilerplateGenRes = std::expected<std::filesystem::path, QString>;

public:
  const std::vector<CommandBoilerplate> &commandBoilerplates() const;

  BoilerplateGenRes generate(const std::filesystem::path &targetDir,
                             const ExtensionBoilerplateConfig &config);

  ExtensionBoilerplateGenerator();
};
