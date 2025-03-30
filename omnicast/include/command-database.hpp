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

enum CommandMode { CommandModeView, CommandModeNoView };
enum CommandType { CommandTypeBuiltin, CommandTypeExtension };

class AbstractCommand {
  QString _id;
  QString _name;
  std::optional<OmniIconUrl> _url;
  OmniIconUrl _repositoryIcon;
  QString _repositoryId;
  QString _repositoryName;
  std::vector<Preference> _preferences;

public:
  virtual QString id() { return _repositoryId + "." + _id; }
  virtual QString name() const { return _name; }

  virtual OmniIconUrl iconUrl() const {
    if (_url) return *_url;
    return _repositoryIcon;
  }

  void setIconUrl(const OmniIconUrl &icon) { _url = icon; }
  void setRepositoryIconUrl(const OmniIconUrl &icon) { _repositoryIcon = icon; }
  void setRepositoryId(const QString &id) { _repositoryId = id; }
  void setRepositoryName(const QString &name) { _repositoryName = name; }

  const QString &repositoryName() { return _repositoryName; }
  const QString &repositoryId() { return _repositoryId; }

  virtual CommandMode type() const = 0;
  virtual std::vector<QString> keywords() const { return {}; }
  virtual std::vector<Preference> preferences() const { return _preferences; }
  virtual std::vector<Argument> arguments() const { return {}; }

  AbstractCommand(const QString &id, const QString &name, const std::optional<OmniIconUrl> &url)
      : _id(id), _name(name), _url(url), _preferences({}) {}
};

class AbstractViewCommand : public AbstractCommand {
public:
  virtual ViewCommandContext *createContext(AppWindow &app, const std::shared_ptr<AbstractCommand> &command,
                                            const QString &query) const = 0;
  CommandMode type() const { return CommandMode::CommandModeView; }

  AbstractViewCommand(const QString &id, const QString &name, const std::optional<OmniIconUrl> &url)
      : AbstractCommand(id, name, url) {}
};

class AbstractNoViewCommand : public AbstractCommand {
  virtual void exec(AppWindow &app) = 0;
  CommandMode type() const override { return CommandMode::CommandModeNoView; }
};

template <typename T> class BuiltinViewCommand : public AbstractViewCommand {
public:
  ViewCommandContext *createContext(AppWindow &app, const std::shared_ptr<AbstractCommand> &command,
                                    const QString &query) const override {
    return new SingleViewCommand<T>(&app, command);
  }

  BuiltinViewCommand(const QString &id, const QString &name, const std::optional<OmniIconUrl> &url)
      : AbstractViewCommand(id, name, url) {}
};

class CommandBuilder {
  QString _id;
  QString _name;
  std::optional<OmniIconUrl> _url;

public:
  CommandBuilder(const QString &id) : _id(id) {}

  CommandBuilder &withName(const QString &name) {
    _name = name;
    return *this;
  }
  CommandBuilder &withIcon(const OmniIconUrl &url) {
    _url = url;
    return *this;
  }

  CommandBuilder &withTintedIcon(const QString &name, ColorTint tint) {
    _url = BuiltinOmniIconUrl(name).setBackgroundTint(tint);
    return *this;
  }

  CommandBuilder &withPreference(const Preference &preference) {}

  template <typename T> std::shared_ptr<AbstractCommand> toView() {
    return std::make_shared<BuiltinViewCommand<T>>(_id, _name, _url);
  }
};

class AbstractCommandRepository {
public:
  virtual QString id() const = 0;
  virtual QString name() const = 0;
  virtual std::vector<std::shared_ptr<AbstractCommand>> commands() const = 0;
  virtual OmniIconUrl iconUrl() const = 0;
  virtual std::vector<Preference> preferences() const { return {}; }
};

class BuiltinCommandRepository : public AbstractCommandRepository {
  QString _id;
  QString _name;
  std::vector<std::shared_ptr<AbstractCommand>> _commands;
  OmniIconUrl _icon;
  std::vector<Preference> _preferences;

  QString id() const override { return _id; }
  QString name() const override { return _name; }
  std::vector<std::shared_ptr<AbstractCommand>> commands() const override { return _commands; }
  OmniIconUrl iconUrl() const override { return _icon; }
  std::vector<Preference> preferences() const override { return _preferences; }

public:
  BuiltinCommandRepository(const QString &id, const QString &name,
                           const std::vector<std::shared_ptr<AbstractCommand>> &commands,
                           const OmniIconUrl &url, const std::vector<Preference> &preferences)
      : _id(id), _name(name), _commands(commands), _icon(url), _preferences(preferences) {}
};

class CommandRepositoryBuilder {
  QString _id;
  QString _name;
  OmniIconUrl _icon;
  std::vector<std::shared_ptr<AbstractCommand>> _commands;

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
  CommandRepositoryBuilder &withCommand(const std::shared_ptr<AbstractCommand> &cmd) {
    cmd->setRepositoryIconUrl(_icon);
    cmd->setRepositoryId(_id);
    cmd->setRepositoryName(_name);
    _commands.push_back(cmd);
    return *this;
  }

  std::shared_ptr<AbstractCommandRepository> makeShared() {
    return std::make_shared<BuiltinCommandRepository>(_id, _name, _commands, _icon,
                                                      std::vector<Preference>{});
  };
};

class CommandDatabase {
  std::vector<std::shared_ptr<AbstractCommandRepository>> _repositories;

public:
  const std::vector<std::shared_ptr<AbstractCommandRepository>> &repositories() const;
  void registerRepository(const std::shared_ptr<AbstractCommandRepository> &repo);

  const AbstractCommand *findCommand(const QString &id);
  const AbstractCommandRepository *findRepository(const QString &name);

  CommandDatabase();
};
