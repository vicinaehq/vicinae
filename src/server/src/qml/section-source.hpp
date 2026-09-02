#pragma once
#include <QMimeData>
#include <QVariantList>

#include <functional>
#include <memory>
#include <optional>
#include <string_view>

#include "ui/list-accessory/list-accessory.hpp"
#include "view-scope.hpp"
#include "view-utils.hpp"
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
  virtual std::optional<ImageURL> itemIcon(int i) const = 0;
  virtual AccessoryList itemAccessories(int) const { return {}; }
  virtual QVariantList itemShortcutTokens(int) const { return {}; }

  virtual bool isDraggable(int) const { return false; }
  virtual std::unique_ptr<QMimeData> dragMimeData(int) const { return {}; }

  virtual QVariant customData(int, int) const { return {}; }
  virtual QHash<int, QByteArray> customRoleNames() const { return {}; }
  virtual QHash<int, QVariant> customRoleDefaults() const { return {}; }

  virtual std::unique_ptr<ActionPanelState> actionPanel(int i) const = 0;
  virtual void onSelected(int) {}
  virtual void onSelectionCleared() {}

  virtual void setFilter(std::string_view) {}

  void notifyChanged() {
    if (m_onChange) m_onChange(false);
  }

  void notifyItemsRefreshed() {
    if (m_onChange) m_onChange(true);
  }

  void setOnChanged(std::function<void(bool)> cb) { m_onChange = std::move(cb); }

  QString imageSourceFor(const ImageURL &url) const { return qml::imageSourceFor(url); }

private:
  ViewScope m_scope;
  std::function<void(bool)> m_onChange;
};
