#pragma once
#include "argument.hpp"
#include "command-database.hpp"
#include "command.hpp"
#include "omni-icon.hpp"
#include "preference.hpp"
#include "ui/action-pannel/action.hpp"
#include <memory>
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

  const QString &extensionId() const;
  void setExtensionId(const QString &text);

  const QString &extensionIcon() const;
  void setExtensionIcon(const QString &icon);

  void setExtensionPreferences(const PreferenceList &prefs) { _extensionPreferences = prefs; }

  std::vector<Argument> arguments() const override { return m_arguments; }
  std::vector<std::shared_ptr<BasePreference>> preferences() const override {
    PreferenceList list;

    list.reserve(_extensionPreferences.size() + _preferences.size());
    list.insert(list.end(), _extensionPreferences.begin(), _extensionPreferences.end());
    list.insert(list.end(), _preferences.begin(), _preferences.end());

    return list;
  }

  QString id() const override;
  QString name() const override;

  OmniIconUrl iconUrl() const override;
  QString repositoryName() const override;

  void setAssetPath(const std::filesystem::path &path);
  void setExtensionTitle(const QString &title);
  CommandType type() const override;
  CommandMode mode() const override;

  CommandContext *createContext(AppWindow &app, const std::shared_ptr<AbstractCmd> &command,
                                const QString &query) const override;

  ExtensionCommand() {}
};
