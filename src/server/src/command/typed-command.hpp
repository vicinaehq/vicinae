#pragma once
#include <utility>
#include <vector>
#include "command/command-controller.hpp"
#include "command/command-database.hpp"
#include "command/preference-schema.hpp"
#include "command/single-view-command-context.hpp"

template <TypedPreferences P, TypedPreferences R = NoPreferences> class TypedCommandController {
public:
  explicit TypedCommandController(CommandController &controller, P defaults = {})
      : m_controller(controller),
        m_preferences(readPreferences<P>(controller.preferenceValues(), std::move(defaults))),
        m_repositoryPreferences(controller.preferences<R>()) {}

  const P &preferences() const { return m_preferences; }
  const R &repositoryPreferences() const { return m_repositoryPreferences; }

  const LaunchProps &launchProps() const { return m_controller.launchProps(); }
  const AbstractCmd &info() const { return m_controller.info(); }
  ScopedLocalStorage storage() const { return m_controller.storage(); }
  const ApplicationContext *context() const { return m_controller.context(); }
  CommandController &raw() const { return m_controller; }

private:
  CommandController &m_controller;
  P m_preferences;
  R m_repositoryPreferences;
};

template <TypedPreferences P, TypedPreferences R = NoPreferences, typename Base = BuiltinCallbackCommand>
class TypedCallbackCommand : public Base {
public:
  using Controller = TypedCommandController<P, R>;

  std::vector<Preference> preferences() const override {
    return describePreferences<P>(defaultPreferences());
  }
  void preferenceValuesChanged(const PreferenceValues &values) const override {
    preferencesChanged(readPreferences<P>(values, defaultPreferences()));
  }
  void execute(CommandController &controller) const override {
    execute(Controller{controller, defaultPreferences()});
  }

  virtual P defaultPreferences() const { return {}; }
  virtual void execute(const Controller &controller) const = 0;
  virtual void preferencesChanged(const P &) const {}
};

template <DerivedFromView T, TypedPreferences P, TypedPreferences R = NoPreferences>
class TypedViewCommand : public BuiltinViewCommand<T> {
public:
  std::vector<Preference> preferences() const override { return describePreferences<P>(); }
  void preferenceValuesChanged(const PreferenceValues &values) const override {
    preferencesChanged(readPreferences<P>(values));
  }

  virtual void preferencesChanged(const P &) const {}
};

template <TypedPreferences R> class TypedCommandRepository : public BuiltinCommandRepository {
public:
  std::vector<Preference> preferences() const override { return describePreferences<R>(); }
  void initialized(const PreferenceValues &values) const override { initialized(readPreferences<R>(values)); }
  void preferenceValuesChanged(const PreferenceValues &values) const override {
    preferencesChanged(readPreferences<R>(values));
  }

  virtual void initialized(const R &) const {}
  virtual void preferencesChanged(const R &) const {}
};
