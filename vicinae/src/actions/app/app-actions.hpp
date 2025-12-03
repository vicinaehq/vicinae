#pragma once
#include "services/app-service/abstract-app-db.hpp"
#include "ui/action-pannel/action.hpp"

class OpenAppAction : public AbstractAction {

public:
  void setClearSearch(bool value) { m_clearSearch = value; }
  void execute(ApplicationContext *context) override;

  OpenAppAction(const std::shared_ptr<AbstractApplication> &app, const QString &title,
                const std::vector<QString> args);

private:
  std::shared_ptr<AbstractApplication> application;
  std::vector<QString> args;
  bool m_clearSearch = false;
};

class OpenInTerminalAction : public AbstractAction {
public:
  void setClearSearch(bool value) { m_clearSearch = value; }
  void execute(ApplicationContext *context) override;

  QString title() const override {
    return m_title.value_or(QString("Open in %1").arg(m_emulator->displayName()));
  }
  void setTitle(const QString &title) { m_title = title; }

  std::optional<ImageURL> icon() const override { return m_emulator->iconUrl(); }

  OpenInTerminalAction(const std::shared_ptr<AbstractApplication> &emulator,
                       const std::vector<QString> &cmdline,
                       const LaunchTerminalCommandOptions &opts = {.hold = true});

private:
  std::shared_ptr<AbstractApplication> m_emulator;
  std::vector<QString> m_args;
  std::optional<QString> m_title;
  LaunchTerminalCommandOptions m_opts;
  bool m_clearSearch = true;
};

class OpenRawProgramAction : public AbstractAction {
  QString m_prog;
  std::vector<QString> m_args;
  bool m_clearSearch = false;

  void execute(ApplicationContext *context) override;

public:
  void setClearSearch(bool value) { m_clearSearch = value; }
  QString title() const override { return "Execute program"; }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin("terminal"); }

  OpenRawProgramAction(const QString &prog, const std::vector<QString> args);
};
