#pragma once
#include "services/local-storage/scoped-local-storage.hpp"
#include "command/command.hpp"
#include "command/preference-schema.hpp"

class CommandController {
public:
  CommandController(ApplicationContext &ctx, const AbstractCmd &cmd, const LaunchProps &props);

  const LaunchProps &launchProps() const;
  const AbstractCmd &info() const;
  ScopedLocalStorage storage() const;
  PreferenceValues preferenceValues() const;
  template <TypedPreferences P> P preferences() const { return readPreferences<P>(preferenceValues()); }
  void setPreferenceValues(const PreferenceValues &value) const;

  /**
   * Access the global application context.
   */
  ApplicationContext const *context() const;

private:
  ApplicationContext &m_ctx;
  const AbstractCmd &m_cmd;
  LaunchProps m_props;
};
