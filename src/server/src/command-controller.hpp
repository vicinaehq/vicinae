#pragma once
#include "services/local-storage/scoped-local-storage.hpp"
#include "command.hpp"

class CommandController {
public:
  CommandController(ApplicationContext &ctx, const AbstractCmd &cmd, const LaunchProps &props);

  const LaunchProps &launchProps() const;
  const AbstractCmd &info() const;
  ScopedLocalStorage storage() const;
  QJsonObject preferenceValues() const;
  void setPreferenceValues(const QJsonObject &value) const;

  /**
   * Access the global application context.
   */
  ApplicationContext const *context() const;

private:
  ApplicationContext &m_ctx;
  const AbstractCmd &m_cmd;
  LaunchProps m_props;
};
