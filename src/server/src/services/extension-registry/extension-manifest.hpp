#pragma once
#include <chrono>
#include <expected>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include "command/argument.hpp"
#include "command/command-types.hpp"
#include "command/preference.hpp"

struct ManifestError {
  QString m_message;

  ManifestError(const QString &message) : m_message(message) {}
};

struct ExtensionManifest {
  enum class Provenance { Local, Vicinae, Raycast };

  struct Command {
    QString name;
    QString title;
    QString description;
    CommandMode mode;
    std::vector<Preference> preferences;
    std::vector<QString> keywords;
    std::vector<CommandArgument> arguments;
    std::optional<QString> icon;
    std::optional<std::chrono::seconds> interval;
    std::filesystem::path entrypoint;
    bool defaultDisabled;
    Provenance provenance;
  };

  /**
   * An AI tool, as emitted by the extension build step: the input schema is generated from the
   * TypeScript signature of the tool's default export.
   */
  struct Tool {
    std::string name;
    QString title;
    QString description;
    std::optional<std::string> instructions;
    std::optional<QString> icon;
    std::optional<std::string> inputSchema;
    bool confirmation = false;
    std::filesystem::path entrypoint;
  };

  std::filesystem::path path;
  QString id;
  QString name;
  QString title;
  QString description;
  QString icon;
  QString author;
  std::vector<QString> categories;
  std::vector<Preference> preferences;
  std::vector<Command> commands;
  std::vector<Tool> tools;
  bool needsRaycastApi = false;
  Provenance provenance;

  static std::expected<ExtensionManifest, ManifestError> fromPackageJson(const std::filesystem::path &path);

  bool isFromRaycastStore() const { return provenance == ExtensionManifest::Provenance::Raycast; }
  bool isFromVicinaeStore() const { return provenance == ExtensionManifest::Provenance::Vicinae; }
  bool isLocal() const { return provenance == ExtensionManifest::Provenance::Local; }
};
