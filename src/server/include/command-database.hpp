#pragma once
#include "argument.hpp"
#include "command.hpp"
#include "ui/image/url.hpp"
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

template <typename T>
concept DerivedFromCommand = std::derived_from<T, AbstractCmd>;

class BuiltinCommand : public AbstractCmd {
  std::optional<ImageURL> _url;
  QString _repositoryId;
  QString _repositoryName;

public:
  EntrypointId uniqueId() const override final {
    return EntrypointId{_repositoryId.toStdString(), id().toStdString()};
  }

  virtual QString id() const = 0;
  QString name() const override = 0;
  QString description() const override { return ""; }
  ImageURL iconUrl() const override = 0;
  virtual CommandType type() const override final { return CommandTypeBuiltin; }
  QString extensionId() const override { return _repositoryId; }
  QString commandId() const override { return id(); }

  QString repositoryName() const override { return _repositoryId; }

  QString author() const override { return Omnicast::APP_ID; }
  QString authorSuffixed() const override { return Omnicast::APP_ID; }

  // TODO: remove
  void setRepositoryIconUrl(const ImageURL &icon) {}

  void setRepositoryId(const QString &id) { _repositoryId = id; }
  void setRepositoryName(const QString &name) { _repositoryName = name; }

  bool isFallback() const override { return false; }
  void setIconUrl(const ImageURL &url) { _url = url; }

  QString repositoryDisplayName() const override { return _repositoryName; }
  const QString &repositoryId() { return _repositoryId; }

  virtual std::vector<QString> keywords() const override { return {}; }

  virtual ~BuiltinCommand() = default;
};

class BuiltinCommandRepository : public AbstractCommandRepository {
  std::vector<std::shared_ptr<AbstractCmd>> _commands;

  virtual QString id() const override = 0;
  QString displayName() const override = 0;
  std::vector<std::shared_ptr<AbstractCmd>> commands() const override final { return _commands; }
  QString author() const override final { return Omnicast::APP_ID; }

protected:
  template <DerivedFromCommand T> void registerCommand() {
    auto cmd = std::make_shared<T>();
    cmd->setRepositoryId(id());
    cmd->setRepositoryName(displayName());
    _commands.emplace_back(cmd);
  }
};

class CommandDatabase {
  std::vector<std::shared_ptr<AbstractCommandRepository>> _repositories;

public:
  const std::vector<std::shared_ptr<AbstractCommandRepository>> &repositories() const;

  template <typename T> void registerRepository() { _repositories.push_back(std::make_shared<T>()); }

  const AbstractCommandRepository *findRepository(const QString &name);

  CommandDatabase();
};
