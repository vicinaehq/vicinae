#pragma once
#include "argument.hpp"
#include "argument.hpp"
#include "command.hpp"
#include "omni-icon.hpp"
#include "preference.hpp"
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

class ExtensionCommand : public AbstractCmd {
  QString _extensionId;
  QString _id;
  QString _extensionTitle;
  QString _title;
  QString _icon;
  QString _extensionIcon;
  PreferenceList _extensionPreferences;
  PreferenceList _preferences;
  ArgumentList m_arguments;
  std::filesystem::path _assetPath;
  CommandMode _mode;

  ExtensionCommand(const QJsonObject &obj);

public:
  static ExtensionCommand fromJson(const QJsonObject &obj);

  QString extensionId() const override;
  void setExtensionId(const QString &text);

  const QString &extensionIcon() const;
  void setExtensionIcon(const QString &icon);

  QString description() const override { return ""; }

  void setExtensionPreferences(const PreferenceList &prefs) { _extensionPreferences = prefs; }

  bool isFallback() const override { return true; }

  std::vector<CommandArgument> arguments() const override { return m_arguments; }
  std::vector<Preference> preferences() const override { return _preferences; }

  QString uniqueId() const override;
  QString name() const override;
  QString commandId() const override;

  OmniIconUrl iconUrl() const override;
  QString repositoryName() const override;

  void setAssetPath(const std::filesystem::path &path);
  void setExtensionTitle(const QString &title);
  CommandType type() const override;
  CommandMode mode() const override;

  CommandContext *createContext(const std::shared_ptr<AbstractCmd> &command) const override;

  ExtensionCommand() {}
};
