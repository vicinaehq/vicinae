#pragma once
#include "command-database.hpp"

template <typename T> class SingleViewCommand : public CommandContext {
public:
  SingleViewCommand(AppWindow *app, const std::shared_ptr<AbstractCmd> &command)
      : CommandContext(app, command) {}
  void load() override {
    qDebug() << "loading single view";
    return app()->pushView(new T(*app()));
  }
};

template <typename T> class BuiltinViewCommand : public BuiltinCommand {
public:
  CommandMode mode() const override { return CommandMode::CommandModeView; }

  CommandContext *createContext(AppWindow &app, const std::shared_ptr<AbstractCmd> &command,
                                const QString &query) const override {
    return new SingleViewCommand<T>(&app, command);
  }

  BuiltinViewCommand(const QString &id, const QString &name,
                     const std::optional<OmniIconUrl> &url = std::nullopt)
      : BuiltinCommand(id, name, url) {}
};
