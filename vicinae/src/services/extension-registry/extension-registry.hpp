#pragma once
#include "common.hpp"
#include "omni-command-db.hpp"
#include "preference.hpp"
#include "services/local-storage/local-storage-service.hpp"
#include <expected>
#include <filesystem>
#include <qjsonobject.h>
#include <qobject.h>
#include <qtmetamacros.h>
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
    bool defaultDisabled;
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

class ExtensionRegistry : public QObject {
  Q_OBJECT

  OmniCommandDatabase &m_db;
  LocalStorageService &m_storage;

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
  bool uninstall(const QString &id);

  ExtensionRegistry(OmniCommandDatabase &commandDb, LocalStorageService &storage);

signals:
  void extensionAdded(const QString &id);
  void extensionUninstalled(const QString &id);
};
