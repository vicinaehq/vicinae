#pragma once
#include "argument.hpp"
#include "common.hpp"
#include "vicinae.hpp"
#include "preference.hpp"
#include "ui/action-pannel/action.hpp"
#include <endian.h>
#include <qobject.h>
#include <qdebug.h>
#include "common/entrypoint.hpp"

class View;
class LaunchProps;

class AbstractCmd {
public:
  virtual ~AbstractCmd() = default;
  virtual EntrypointId uniqueId() const = 0;
  virtual QString name() const = 0;
  virtual QString description() const = 0;
  virtual ImageURL iconUrl() const = 0;
  virtual CommandType type() const = 0;
  virtual CommandMode mode() const = 0;
  virtual QString author() const = 0;
  virtual QString authorSuffixed() const = 0;
  virtual std::vector<Preference> preferences() const { return {}; }
  virtual std::vector<CommandArgument> arguments() const { return {}; }
  virtual std::vector<QString> keywords() const { return {}; }
  virtual QString repositoryDisplayName() const { return ""; }
  virtual QString repositoryName() const { return ""; }
  virtual bool isFallback() const { return false; }
  virtual void preferenceValuesChanged(const QJsonObject &value) const {}
  virtual bool isInternal() const { return false; }

  /**
   * Optional override of the navigation title that is to be shown when
   * the command is execute from the root search. This only applies to view commands.
   * The default is to use the name of the command.
   */
  virtual QString navigationTitle() const { return name(); }
  virtual ImageURL navigationIcon() const { return iconUrl(); }

  virtual bool isDefaultDisabled() const { return false; }

  QString deeplink() const {
    return QString("%1://extensions/%2/%3/%4")
        .arg(Omnicast::APP_SCHEME)
        .arg(author())
        .arg(repositoryName())
        .arg(commandId());
  }

  virtual QString extensionId() const = 0;
  virtual QString commandId() const = 0;

  bool isView() const { return mode() == CommandModeView; }
  bool isNoView() const { return mode() == CommandModeNoView; }

  virtual CommandContext *createContext(const std::shared_ptr<AbstractCmd> &command) const { return nullptr; }
};

class AbstractCommandRepository {
public:
  virtual QString id() const = 0;
  virtual QString displayName() const = 0;
  virtual QString name() const { return id(); }
  virtual QString description() const { return ""; }
  virtual QString author() const = 0;
  virtual std::vector<std::shared_ptr<AbstractCmd>> commands() const = 0;
  virtual ImageURL iconUrl() const = 0;
  virtual std::vector<Preference> preferences() const { return {}; }
  virtual QWidget *settingsDetail() const { return new QWidget; }

  /**
   * Triggered the first time the provider is registered
   */
  virtual void initialized(const QJsonObject &preferences) const {}

  virtual void preferenceValuesChanged(const QJsonObject &value) const {}

  virtual ~AbstractCommandRepository() = default;
};

class CommandContext : public QObject {
  Q_OBJECT

  std::shared_ptr<AbstractCmd> _cmd;
  ApplicationContext *m_ctx = nullptr;

public:
  const AbstractCmd *command() const { return _cmd.get(); }
  virtual void onActionExecuted(AbstractAction *action) {}

  void setContext(ApplicationContext *ctx) { m_ctx = ctx; }
  ApplicationContext *context() const { return m_ctx; }

  virtual void load(const LaunchProps &props) {}
  virtual void unload() {};

  CommandContext(const std::shared_ptr<AbstractCmd> &command) : _cmd(command) {}
};
