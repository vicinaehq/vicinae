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

class AppWindow;
class ViewCommandContext;

enum CommandMode { CommandModeInvalid, CommandModeView, CommandModeNoView, CommandModeMenuBar };
enum CommandType { CommandTypeBuiltin, CommandTypeExtension };

class AbstractCmd {
public:
  virtual QString id() const = 0;
  virtual QString name() const = 0;
  virtual OmniIconUrl iconUrl() const = 0;
  virtual CommandType type() const = 0;
  virtual CommandMode mode() const = 0;
  virtual std::vector<std::shared_ptr<BasePreference>> preferences() const { return {}; }
  virtual std::vector<Argument> arguments() const { return {}; }
  virtual std::vector<QString> keywords() const { return {}; }
  virtual QString repositoryName() const { return ""; }

  virtual CommandContext *createContext(AppWindow &app, const std::shared_ptr<AbstractCmd> &command,
                                        const QString &query) const {
    return nullptr;
  }

  virtual void exec(AppWindow &app) {}
};

class BuiltinCommand : public AbstractCmd {
  QString _id;
  QString _name;
  std::optional<OmniIconUrl> _url;
  OmniIconUrl _repositoryIcon;
  QString _repositoryId;
  QString _repositoryName;
  std::vector<std::shared_ptr<BasePreference>> _preferences;
  std::vector<std::shared_ptr<BasePreference>> _repositoryPreferences;
  std::vector<Argument> _arguments;

public:
  QString id() const override { return _repositoryId + "." + _id; }
  QString name() const override { return _name; }
  OmniIconUrl iconUrl() const override { return _url.value_or(_repositoryIcon); }
  virtual CommandType type() const override { return CommandTypeBuiltin; }

  void setRepositoryIconUrl(const OmniIconUrl &icon) { _repositoryIcon = icon; }
  void setRepositoryId(const QString &id) { _repositoryId = id; }
  void setRepositoryName(const QString &name) { _repositoryName = name; }
  void setRepositoryPreferences(const PreferenceList &prefs) { _repositoryPreferences = prefs; }

  std::vector<std::shared_ptr<BasePreference>> preferences() const override {
    PreferenceList list;

    list.reserve(_repositoryPreferences.size() + _preferences.size());
    list.insert(list.end(), _repositoryPreferences.begin(), _repositoryPreferences.end());
    list.insert(list.end(), _preferences.begin(), _preferences.end());

    return list;
  }
  std::vector<Argument> arguments() const override { return _arguments; }

  void setIconUrl(const OmniIconUrl &url) { _url = url; }

  void setPreferences(const std::vector<std::shared_ptr<BasePreference>> &preferences) {
    _preferences = preferences;
  }
  void setArguments(const std::vector<Argument> &arguments) { _arguments = arguments; }

  QString repositoryName() const override { return _repositoryName; }
  const QString &repositoryId() { return _repositoryId; }

  virtual std::vector<QString> keywords() const override { return {}; }

  BuiltinCommand(const QString &id, const QString &name, const std::optional<OmniIconUrl> &url)
      : _id(id), _name(name), _url(url) {}
};

class AbstractCommandRepository {
public:
  virtual QString id() const = 0;
  virtual QString name() const = 0;
  virtual std::vector<std::shared_ptr<AbstractCmd>> commands() const = 0;
  virtual OmniIconUrl iconUrl() const = 0;
  virtual std::vector<std::shared_ptr<BasePreference>> preferences() const { return {}; }
};

class BuiltinCommandRepository : public AbstractCommandRepository {
  QString _id;
  QString _name;
  std::vector<std::shared_ptr<AbstractCmd>> _commands;
  OmniIconUrl _icon;
  std::vector<std::shared_ptr<BasePreference>> _preferences;

  QString id() const override { return _id; }
  QString name() const override { return _name; }
  std::vector<std::shared_ptr<AbstractCmd>> commands() const override { return _commands; }
  OmniIconUrl iconUrl() const override { return _icon; }
  std::vector<std::shared_ptr<BasePreference>> preferences() const override { return _preferences; }

public:
  BuiltinCommandRepository(const QString &id, const QString &name,
                           const std::vector<std::shared_ptr<AbstractCmd>> &commands, const OmniIconUrl &url,
                           const std::vector<std::shared_ptr<BasePreference>> &preferences)
      : _id(id), _name(name), _commands(commands), _icon(url), _preferences(preferences) {}
};

class CommandRepositoryBuilder {
  QString _id;
  QString _name;
  OmniIconUrl _icon;
  std::vector<std::shared_ptr<AbstractCmd>> _commands;
  std::vector<std::shared_ptr<BasePreference>> _preferences;

public:
  CommandRepositoryBuilder(const QString &id) : _id(id) {}

  CommandRepositoryBuilder &withName(const QString &name) {
    _name = name;
    return *this;
  }
  CommandRepositoryBuilder &withTintedIcon(const QString &name, ColorTint tint) {
    _icon = BuiltinOmniIconUrl(name).setBackgroundTint(tint);
    return *this;
  }
  CommandRepositoryBuilder &withIcon(const OmniIconUrl &icon) {
    _icon = icon;
    return *this;
  }
  CommandRepositoryBuilder &withPreference(const std::shared_ptr<BasePreference> &preference) {
    _preferences.push_back(preference);
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
