#include "services/extension-boilerplate-generator/extension-boilerplate-generator.hpp"
#include <filesystem>

namespace fs = std::filesystem;

std::expected<void, QString>
ExtensionBoilerplateGenerator::generate(const fs::path &targetDir, const ExtensionBoilerplateConfig &config) {
  std::error_code ec;

  if (!fs::is_directory(targetDir, ec)) {
    return std::unexpected(QString(
        "%1 is not a directory. The boilerplate generator will not create the containing directory for you"));
  }

  return std::unexpected("Not implemented");
}

ExtensionBoilerplateGenerator::ExtensionBoilerplateGenerator() {}
