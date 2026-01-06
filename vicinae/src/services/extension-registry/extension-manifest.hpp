#pragma once
#include <expected>
#include <filesystem>
#include "argument.hpp"
#include "common.hpp"
#include "preference.hpp"

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
    std::vector<CommandArgument> arguments;
    std::optional<QString> icon;
    std::filesystem::path entrypoint;
    bool defaultDisabled;
    Provenance provenance;
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
  bool needsRaycastApi = false;
  Provenance provenance;

  static std::expected<ExtensionManifest, ManifestError> fromPackageJson(const std::filesystem::path &path);

  bool isFromRaycastStore() const { return provenance == ExtensionManifest::Provenance::Raycast; }
  bool isFromVicinaeStore() const { return provenance == ExtensionManifest::Provenance::Vicinae; }
  bool isLocal() const { return provenance == ExtensionManifest::Provenance::Local; }

private:
  static CommandArgument parseArgumentFromObject(const QJsonObject &obj);
  static Preference parsePreferenceFromObject(const QJsonObject &obj);
  static Command parseCommandFromObject(const QJsonObject &obj);
};
