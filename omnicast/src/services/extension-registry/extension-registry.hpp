#pragma once
#include "common.hpp"
#include "omni-command-db.hpp"
#include "preference.hpp"
#include <expected>
#include <filesystem>
#include <qjsonobject.h>
#include <vector>
#include <QString>

struct ManifestError {
  QString m_message;

  ManifestError(const QString &message) : m_message(message) {}
};

struct ExtensionManifest {
  struct Command {
    QString name;
    QString title;
    QString description;
    CommandMode mode;
    std::vector<Preference> preferences;
    std::vector<CommandArgument> arguments;
    std::optional<QString> icon;
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
};

class ExtensionRegistry {
  OmniCommandDatabase &m_db;

  CommandArgument parseArgumentFromObject(const QJsonObject &obj);
  Preference parsePreferenceFromObject(const QJsonObject &obj);
  ExtensionManifest::Command parseCommandFromObject(const QJsonObject &obj);

  std::filesystem::path extensionDir() const;

public:
  bool installFromZip(const QString &id, std::string_view data);

  std::expected<ExtensionManifest, ManifestError> scanBundle(const std::filesystem::path &path);
  std::vector<ExtensionManifest> scanAll();
  void rescanBundle();
  bool isInstalled(const QString &id) const;

  ExtensionRegistry(OmniCommandDatabase &commandDb);

signals:
  void extensionAdded(const QString &id);
};
