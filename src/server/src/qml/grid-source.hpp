#pragma once
#include "view-scope.hpp"
#include "view-utils.hpp"
#include <QVariant>
#include <functional>
#include <memory>
#include <optional>
#include <string_view>

class ActionPanelState;

class GridSource {
public:
  virtual ~GridSource() = default;

  void setScope(const ViewScope &scope) { m_scope = scope; }
  const ViewScope &scope() const { return m_scope; }

  virtual QString sectionName() const = 0;
  virtual int count() const = 0;
  virtual std::optional<int> columns() const { return std::nullopt; }
  virtual std::optional<double> aspectRatio() const { return std::nullopt; }

  virtual std::unique_ptr<ActionPanelState> actionPanel(int i) const = 0;
  virtual void onSelected(int) {}
  virtual void onSelectionCleared() {}
  virtual void setFilter(std::string_view) {}

  virtual QVariant customData(int, int) const { return {}; }
  virtual QHash<int, QByteArray> customRoleNames() const { return {}; }

  void notifyChanged() {
    if (m_onChange) m_onChange();
  }

  void setOnChanged(std::function<void()> cb) { m_onChange = std::move(cb); }

  QString imageSourceFor(const ImageURL &url) const { return qml::imageSourceFor(url); }

private:
  ViewScope m_scope;
  std::function<void()> m_onChange;
};
