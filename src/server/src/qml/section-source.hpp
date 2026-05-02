#pragma once
#include "view-scope.hpp"
#include "view-utils.hpp"
#include <QVariantList>
#include <functional>
#include <memory>
#include <string_view>

class ActionPanelState;

class SectionSource {
public:
  virtual ~SectionSource() = default;

  void setScope(const ViewScope &scope) { m_scope = scope; }
  const ViewScope &scope() const { return m_scope; }

  virtual QString sectionName() const = 0;
  virtual int count() const = 0;

  virtual QString itemId(int i) const { return itemTitle(i); }
  virtual QString itemTitle(int i) const = 0;
  virtual QString itemSubtitle(int) const { return {}; }
  virtual QString itemIconSource(int i) const = 0;
  virtual QVariantList itemAccessories(int) const { return {}; }

  virtual std::unique_ptr<ActionPanelState> actionPanel(int i) const = 0;
  virtual void onSelected(int) {}
  virtual void onSelectionCleared() {}

  virtual void setFilter(std::string_view) {}

  void notifyChanged() {
    if (m_onChange) m_onChange();
  }

  void setOnChanged(std::function<void()> cb) { m_onChange = std::move(cb); }

  QString imageSourceFor(const ImageURL &url) const { return qml::imageSourceFor(url); }

private:
  ViewScope m_scope;
  std::function<void()> m_onChange;
};
