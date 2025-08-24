#pragma once
#include <expected>
#include <qstring.h>
#include <filesystem>

struct ExtensionBoilerplateConfig {
  struct CommandConfig {
    QString title;
    QString subtitle;
    QString description;
  };

  QString author;
  QString title;
  QString description;
  std::vector<CommandConfig> commands;
};

class ExtensionBoilerplateGenerator {
  using BoilerplateGenRes = std::expected<void, QString>;

public:
  BoilerplateGenRes generate(const std::filesystem::path &targetDir,
                             const ExtensionBoilerplateConfig &config);

  ExtensionBoilerplateGenerator();
};
