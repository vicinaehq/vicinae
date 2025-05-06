#pragma once
#include "app.hpp"
#include "command-database.hpp"
#include "preference.hpp"

template <typename T> class SingleViewCommand : public CommandContext {
public:
  SingleViewCommand(AppWindow *app, const std::shared_ptr<AbstractCmd> &command)
      : CommandContext(app, command) {}

  void load(const LaunchProps &props) override {
    qDebug() << "loading single view" << command()->name();
    return app()->pushView(new T(*app()), {.navigation = NavigationStatus{.title = command()->name(),
                                                                          .iconUrl = command()->iconUrl()}});
  }
};

template <typename T> class BuiltinCommandContext : public BuiltinCommand {
public:
  CommandMode mode() const override { return CommandMode::CommandModeView; }
  CommandContext *createContext(AppWindow &app, const std::shared_ptr<AbstractCmd> &command,
                                const QString &query) const override {
    return new T(&app, command);
  }

  BuiltinCommandContext(const QString &id, const QString &name,
                        const std::optional<OmniIconUrl> &url = std::nullopt,
                        const PreferenceList &preferences = {})
      : BuiltinCommand(id, name, url) {
    setPreferences(preferences);
  }
};

template <typename T> class BuiltinNoViewCommandContext : public BuiltinCommand {
public:
  CommandMode mode() const override { return CommandMode::CommandModeNoView; }
  CommandContext *createContext(AppWindow &app, const std::shared_ptr<AbstractCmd> &command,
                                const QString &query) const override {
    return new T(&app, command);
  }

  BuiltinNoViewCommandContext(const QString &id, const QString &name,
                              const std::optional<OmniIconUrl> &url = std::nullopt,
                              const PreferenceList &preferences = {})
      : BuiltinCommand(id, name, url) {
    setPreferences(preferences);
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
                     const std::optional<OmniIconUrl> &url = std::nullopt,
                     const PreferenceList &preferences = {})
      : BuiltinCommand(id, name, url) {
    setPreferences(preferences);
  }
};
