#pragma once
#include "argument.hpp"
#include "argument.hpp"
#include "command.hpp"
#include "../../src/ui/image/url.hpp"
#include "preference.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include <qstring.h>
#include <qboxlayout.h>
#include <qfuturewatcher.h>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qthread.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <optional>

class ExtensionCommand : public AbstractCmd {
  QString _extensionId;
  QString m_extensionName;
  QString _extensionTitle;
  QString _extensionIcon;
  PreferenceList _extensionPreferences;
  std::filesystem::path m_path;
  ExtensionManifest::Command m_command;
  QString m_author;
  std::optional<QString> m_subtitleOverride;

public:
  ExtensionCommand(const ExtensionManifest::Command &command) : m_command(command) {}

  QString author() const override { return m_author; }
  QString authorSuffixed() const override {
    // as we plan on having our own store, we want raycast extension authors
    // to be properly segmented.
    if (isRaycast()) return m_author + "@raycast";
    if (isVicinae()) return m_author + "@vicinae";
    return m_author + "@local";
  }

  QString extensionId() const override;
  void setExtensionId(const QString &text);
  void setAuthor(const QString &author) { m_author = author; }

  const QString &extensionIcon() const;
  void setExtensionIcon(const QString &icon);

  QString description() const override { return ""; }

  void setExtensionPreferences(const PreferenceList &prefs) { _extensionPreferences = prefs; }

  std::filesystem::path path() const { return m_path; }
  std::filesystem::path assetPath() const { return m_path / "assets"; }

  bool isFallback() const override { return true; }

  std::vector<CommandArgument> arguments() const override { return m_command.arguments; }
  std::vector<Preference> preferences() const override { return m_command.preferences; }

  bool isRaycast() const { return m_command.provenance == ExtensionManifest::Provenance::Raycast; }
  bool isVicinae() const { return m_command.provenance == ExtensionManifest::Provenance::Vicinae; }
  bool isLocal() const { return m_command.provenance == ExtensionManifest::Provenance::Local; }

  QString uniqueId() const override;
  QString name() const override;
  QString commandId() const override;

  ImageURL iconUrl() const override;
  QString repositoryDisplayName() const override;
  QString repositoryName() const override;

  bool isDefaultDisabled() const override;

  std::optional<QString> overriddenSubtitle() const { return m_subtitleOverride; }
  void setSubtitleOverride(const std::optional<QString> &subtitle) { m_subtitleOverride = subtitle; }

  void setPath(const std::filesystem::path &path);
  void setExtensionTitle(const QString &title);
  void setExtensionName(const QString &name);
  CommandType type() const override;
  CommandMode mode() const override;

  const ExtensionManifest::Command &manifest() const { return m_command; }

  CommandContext *createContext(const std::shared_ptr<AbstractCmd> &command) const override;

  ExtensionCommand() {}
};
