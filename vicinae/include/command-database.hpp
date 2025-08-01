#pragma once
#include "argument.hpp"
#include "command.hpp"
#include "omni-icon.hpp"
#include "preference.hpp"
#include "theme.hpp"
#include <QKeyEvent>
#include <QString>
#include <memory>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qdir.h>
#include <qicon.h>
#include <qlabel.h>
#include <qlist.h>
#include <qlogging.h>
#include <qwidget.h>
#include "common.hpp"
#include "vicinae.hpp"

class AppWindow;
class ViewCommandContext;

class BuiltinCommand : public AbstractCmd {
  QString _id;
  QString _name;
  QString m_description;
  std::optional<OmniIconUrl> _url;
  OmniIconUrl _repositoryIcon;
  QString _repositoryId;
  QString _repositoryName;
  std::vector<Preference> _preferences;
  std::vector<Preference> _repositoryPreferences;
  std::vector<CommandArgument> _arguments;
  bool m_fallback = false;

public:
  QString uniqueId() const override { return _repositoryId + "." + _id; }
  QString name() const override { return _name; }
  QString description() const override { return m_description; }
  OmniIconUrl iconUrl() const override { return _url.value_or(_repositoryIcon); }
  virtual CommandType type() const override { return CommandTypeBuiltin; }
  QString extensionId() const override { return _repositoryId; }
  QString commandId() const override { return _id; }

  void setRepositoryIconUrl(const OmniIconUrl &icon) { _repositoryIcon = icon; }
  void setRepositoryId(const QString &id) { _repositoryId = id; }
  void setRepositoryName(const QString &name) { _repositoryName = name; }
  void setRepositoryPreferences(const PreferenceList &prefs) { _repositoryPreferences = prefs; }
  bool isFallback() const override { return m_fallback; }
  void setIsFallback(bool value) { m_fallback = value; }

  std::vector<Preference> preferences() const override { return _preferences; }
  std::vector<CommandArgument> arguments() const override { return _arguments; }

  void setIconUrl(const OmniIconUrl &url) { _url = url; }

  void setPreferences(const std::vector<Preference> &preferences) { _preferences = preferences; }
  void setArguments(const std::vector<CommandArgument> &arguments) { _arguments = arguments; }

  void setDescription(const QString &description) { m_description = description; }

  QString repositoryName() const override { return _repositoryName; }
  const QString &repositoryId() { return _repositoryId; }

  virtual std::vector<QString> keywords() const override { return {}; }

  BuiltinCommand(const QString &id, const QString &name, const std::optional<OmniIconUrl> &url)
      : _id(id), _name(name), _url(url) {}
};

class BuiltinCommandRepository : public AbstractCommandRepository {
  QString _id;
  QString _name;
  std::vector<std::shared_ptr<AbstractCmd>> _commands;
  OmniIconUrl _icon;
  std::vector<Preference> _preferences;

  QString id() const override { return _id; }
  QString name() const override { return _name; }
  std::vector<std::shared_ptr<AbstractCmd>> commands() const override { return _commands; }
  OmniIconUrl iconUrl() const override { return _icon; }
  std::vector<Preference> preferences() const override { return _preferences; }
  QString author() const override { return Omnicast::APP_ID; }

public:
  BuiltinCommandRepository() {}
  BuiltinCommandRepository(const QString &id, const QString &name,
                           const std::vector<std::shared_ptr<AbstractCmd>> &commands, const OmniIconUrl &url,
                           const std::vector<Preference> &preferences)
      : _id(id), _name(name), _commands(commands), _icon(url), _preferences(preferences) {}
};

class CommandRepositoryBuilder {
  QString _id;
  QString _name;
  OmniIconUrl _icon;
  std::vector<std::shared_ptr<AbstractCmd>> _commands;
  std::vector<Preference> _preferences;

public:
  CommandRepositoryBuilder(const QString &id) : _id(id) {}

  CommandRepositoryBuilder &withName(const QString &name) {
    _name = name;
    return *this;
  }
  CommandRepositoryBuilder &withTintedIcon(const QString &name, SemanticColor tint) {
    _icon = BuiltinOmniIconUrl(name).setBackgroundTint(tint);
    return *this;
  }
  CommandRepositoryBuilder &withIcon(const OmniIconUrl &icon) {
    _icon = icon;
    return *this;
  }
  CommandRepositoryBuilder &withPreference(const Preference &preference) {
    _preferences.push_back(preference);
    return *this;
  }
  CommandRepositoryBuilder &withCommand(const std::shared_ptr<AbstractCmd> &cmd) {
    _commands.push_back(cmd);
    return *this;
  }
  CommandRepositoryBuilder &withCommand(const std::shared_ptr<BuiltinCommand> &cmd) {
    cmd->setRepositoryIconUrl(_icon);
    cmd->setRepositoryId(_id);
    cmd->setRepositoryName(_name);
    cmd->setRepositoryPreferences(_preferences);
    _commands.push_back(cmd);
    return *this;
  }

  std::shared_ptr<AbstractCommandRepository> makeShared() {
    return std::make_shared<BuiltinCommandRepository>(_id, _name, _commands, _icon, _preferences);
  };
};

class CommandDatabase {
  std::vector<std::shared_ptr<AbstractCommandRepository>> _repositories;

public:
  const std::vector<std::shared_ptr<AbstractCommandRepository>> &repositories() const;
  void registerRepository(const std::shared_ptr<AbstractCommandRepository> &repo);

  const AbstractCmd *findCommand(const QString &id);
  const AbstractCommandRepository *findRepository(const QString &name);

  CommandDatabase();
};
