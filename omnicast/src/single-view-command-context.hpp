#pragma once
#include "command-database.hpp"
#include "preference.hpp"
#include "service-registry.hpp"
#include <concepts>

template <typename T> class SingleViewCommand : public CommandContext {
public:
  SingleViewCommand(const std::shared_ptr<AbstractCmd> &command) : CommandContext(command) {}

  void load(const LaunchProps &props) override {
    qDebug() << "loading single view" << command()->name();
    auto ui = ServiceRegistry::instance()->UI();

    ui->pushView(new T(), {.navigation = NavigationStatus{.title = command()->name(),
                                                          .iconUrl = command()->iconUrl()}});
  }
};

template <typename T>
concept DerivedFromView = std::derived_from<T, BaseView>;

template <DerivedFromView T> class AbstractViewCommand : public AbstractCmd {
public:
  CommandMode mode() const override { return CommandMode::CommandModeView; }
  virtual CommandType type() const override { return CommandType::CommandTypeBuiltin; }
  CommandContext *createContext(const std::shared_ptr<AbstractCmd> &command) const override {
    return new SingleViewCommand<T>(command);
  }
};

template <typename T> class BuiltinCommandContext : public BuiltinCommand {
public:
  CommandMode mode() const override { return CommandMode::CommandModeView; }
  CommandContext *createContext(const std::shared_ptr<AbstractCmd> &command) const override {
    return new T(command);
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
  CommandContext *createContext(const std::shared_ptr<AbstractCmd> &command) const override {
    return new T(command);
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

  CommandContext *createContext(const std::shared_ptr<AbstractCmd> &command) const override {
    return new SingleViewCommand<T>(command);
  }

  BuiltinViewCommand(const QString &id, const QString &name,
                     const std::optional<OmniIconUrl> &url = std::nullopt,
                     const PreferenceList &preferences = {})
      : BuiltinCommand(id, name, url) {
    setPreferences(preferences);
  }
};
