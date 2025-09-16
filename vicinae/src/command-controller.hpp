#pragma once
#include "common.hpp"
#include "services/local-storage/scoped-local-storage.hpp"
#include <qjsonobject.h>
#include <qobject.h>

class CommandController {
public:
  CommandController(ApplicationContext &ctx, const AbstractCmd &cmd, const LaunchProps &props);

  const LaunchProps &launchProps() const;
  const AbstractCmd &info() const;
  ScopedLocalStorage storage() const;
  QJsonObject preferenceValues() const;
  void setPreferenceValues(const QJsonObject &value) const;

private:
  ApplicationContext &m_ctx;
  const AbstractCmd &m_cmd;
  LaunchProps m_props;
};
