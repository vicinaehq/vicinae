#pragma once
#include "command.hpp"
#include "extension/extension.hpp"
#include "omni-icon.hpp"
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
class ViewCommand;

enum CommandType { CommandTypeView, CommandTypeNoView };

class AbstractCommand {
  QString _id;
  QString _name;
  std::optional<OmniIconUrl> _url;
  OmniIconUrl _repositoryIcon;
  QString _repositoryId;
  QString _repositoryName;
  std::vector<Extension::Preference> _preferences;

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

  virtual CommandType type() const = 0;
  virtual std::vector<QString> keywords() const { return {}; }
  virtual std::vector<Extension::Preference> preferences() const { return _preferences; }
  virtual std::vector<Extension::Argument> arguments() const { return {}; }

  AbstractCommand(const QString &id, const QString &name, const std::optional<OmniIconUrl> &url)
      : _id(id), _name(name), _url(url), _preferences({}) {}
};

class AbstractViewCommand : public AbstractCommand {
public:
  virtual ViewCommand *launch(AppWindow &app, const QString &query) const = 0;
  CommandType type() const { return CommandType::CommandTypeView; }

  AbstractViewCommand(const QString &id, const QString &name, const std::optional<OmniIconUrl> &url)
      : AbstractCommand(id, name, url) {}
};

class AbstractNoViewCommand : public AbstractCommand {
  virtual void exec(AppWindow &app) = 0;
  CommandType type() const override { return CommandType::CommandTypeNoView; }
};

template <typename T> class BuiltinViewCommand : public AbstractViewCommand {
  QString _id;
  QString _name;
  OmniIconUrl _url;

public:
  ViewCommand *launch(AppWindow &app, const QString &query) const override {
    return new SingleViewCommand<T>();
  }

  BuiltinViewCommand(const QString &id, const QString &name, const std::optional<OmniIconUrl> &url)
      : AbstractViewCommand(id, name, url) {}
};

template <typename T> class ViewCommandBuilder {
  QString _id;
  QString _name;
  std::optional<OmniIconUrl> _url;

public:
  ViewCommandBuilder(const QString &id) : _id(id) {}

  ViewCommandBuilder &withName(const QString &name) {
    _name = name;
    return *this;
  }
  ViewCommandBuilder &withIcon(const OmniIconUrl &url) {
    _url = url;
    return *this;
  }

  ViewCommandBuilder &withTintedIcon(const QString &name, ColorTint tint) {
    _url = BuiltinOmniIconUrl(name).setBackgroundTint(tint);
    return *this;
  }

  ViewCommandBuilder &withPreference(const Extension::Preference &preference) {}

  std::shared_ptr<AbstractCommand> makeShared() {
    return std::make_shared<BuiltinViewCommand<T>>(_id, _name, _url);
  }
};

class AbstractCommandRepository {
public:
  virtual QString id() const = 0;
  virtual QString name() const = 0;
  virtual std::vector<std::shared_ptr<AbstractCommand>> commands() const = 0;
  virtual OmniIconUrl iconUrl() const = 0;
  virtual std::vector<Extension::Preference> preferences() const { return {}; }
};

class BuiltinCommandRepository : public AbstractCommandRepository {
  QString _id;
  QString _name;
  std::vector<std::shared_ptr<AbstractCommand>> _commands;
  OmniIconUrl _icon;
  std::vector<Extension::Preference> _preferences;

  QString id() const override { return _id; }
  QString name() const override { return _name; }
  std::vector<std::shared_ptr<AbstractCommand>> commands() const override { return _commands; }
  OmniIconUrl iconUrl() const override { return _icon; }
  std::vector<Extension::Preference> preferences() const override { return _preferences; }

public:
  BuiltinCommandRepository(const QString &id, const QString &name,
                           const std::vector<std::shared_ptr<AbstractCommand>> &commands,
                           const OmniIconUrl &url, const std::vector<Extension::Preference> &preferences)
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
                                                      std::vector<Extension::Preference>{});
  };
};

class CommandDatabase {
  std::vector<std::shared_ptr<AbstractCommandRepository>> repositories;

public:
  const std::vector<std::shared_ptr<AbstractCommandRepository>> &list() { return repositories; }
  const AbstractCommand *findById(const QString &id);

  void registerRepository(const std::shared_ptr<AbstractCommandRepository> &repo) {
    repositories.push_back(std::move(repo));
  }

  CommandDatabase();
};
