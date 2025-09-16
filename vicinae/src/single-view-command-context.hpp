#pragma once
#include "command-database.hpp"
#include "command.hpp"
#include "common.hpp"
#include "ui/views/base-view.hpp"
#include "navigation-controller.hpp"
#include <concepts>

template <typename T>
concept DerivedFromView = std::derived_from<T, BaseView>;

template <DerivedFromView T> class SingleViewCommand : public CommandContext {
public:
  SingleViewCommand(const std::shared_ptr<AbstractCmd> &command) : CommandContext(command) {}

  void load(const LaunchProps &props) override {
    auto &nav = context()->navigation;

    nav->pushView(new T());
    nav->setNavigationTitle(command()->navigationTitle());
    nav->setNavigationIcon(command()->navigationIcon());
  }
};

template <DerivedFromView T> class BuiltinViewCommand : public BuiltinCommand {
public:
  CommandMode mode() const override final { return CommandMode::CommandModeView; }
  CommandContext *createContext(const std::shared_ptr<AbstractCmd> &command) const override {
    return new SingleViewCommand<T>(command);
  }
};

class CallbackContext : public CommandContext {
  using Callback = std::function<void(const LaunchProps &props, ApplicationContext *)>;

  Callback m_cb;

public:
  void load(const LaunchProps &props) override { m_cb(props, context()); }

  CallbackContext(const std::shared_ptr<AbstractCmd> &command, const Callback &cb)
      : CommandContext(command), m_cb(cb) {}
};

class BuiltinCallbackCommand : public BuiltinCommand {
public:
  CommandMode mode() const override { return CommandMode::CommandModeNoView; }

  virtual void execute(const LaunchProps &props, ApplicationContext *ctx) const {}

  CommandContext *createContext(const std::shared_ptr<AbstractCmd> &command) const override {
    return new CallbackContext(command, [this](auto &&props, auto &&ctx) { execute(props, ctx); });
  }
};
