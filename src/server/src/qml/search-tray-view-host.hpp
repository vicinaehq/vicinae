#pragma once
#include "builtin_icon.hpp"
#include "common/context.hpp"
#include "fuzzy/fuzzy-searchable.hpp"
#include "mono-list-view-host.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/tray-host/abstract-tray-host.hpp"
#include "theme/colors.hpp"
#include "ui/action-pannel/action-panel-state.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/list-accessory/list-accessory.hpp"
#include <QBuffer>
#include <QCoreApplication>

static QString trayItemTitle(const TrayItem &item) { return item.title.isEmpty() ? item.id : item.title; }

struct TrayMenuRow {
  TrayMenuItem entry;
  QString label;
};

template <> struct fuzzy::FuzzySearchable<TrayItem> {
  static fuzzy::Match score(const TrayItem &item, const fuzzy::Query &query) {
    return fuzzy::scoreWeighted(
        {{trayItemTitle(item).toStdString(), 1.0}, {item.tooltipTitle.toStdString(), 0.5}}, query);
  }
};

template <> struct fuzzy::FuzzySearchable<TrayMenuRow> {
  static fuzzy::Match score(const TrayMenuRow &row, const fuzzy::Query &query) {
    return fuzzy::scoreWeighted({{row.label.toStdString(), 1.0}}, query);
  }
};

class TrayMenuViewHost : public MonoListViewHost<TrayMenuRow> {
  Q_DECLARE_TR_FUNCTIONS(TrayMenuViewHost)

public:
  explicit TrayMenuViewHost(TrayItem item) : m_item(std::move(item)) {}

  void onMount() override {
    setNavigationTitle(trayItemTitle(m_item));
    setSearchPlaceholderText(tr("Search menu..."));
    connect(context()->services->trayHost(), &AbstractTrayHost::menuChanged, this,
            [this](const QString &key) {
              if (key == m_item.key()) reload();
            });
    reload();
  }

  QString displayTitle(const ItemType &row) const override { return row.label; }
  QString displaySubtitle(const ItemType &) const override { return {}; }
  QString displayId(const ItemType &row) const override { return QString::number(row.entry.id); }

  std::optional<ImageURL> displayIcon(const ItemType &row) const override {
    if (!row.entry.iconData.isNull()) {
      QByteArray bytes;
      QBuffer buf(&bytes);
      buf.open(QIODevice::WriteOnly);
      row.entry.iconData.save(&buf, "PNG");
      return ImageURL::rawData(bytes, "image/png");
    }
    if (!row.entry.iconName.isEmpty()) return ImageURL::system(row.entry.iconName);
    return m_item.icon();
  }

  AccessoryList displayAccessories(const ItemType &row) const override {
    if (row.entry.toggleType == TrayMenuItem::ToggleType::None) return {};
    auto icon = row.entry.toggleState == 1
                    ? ImageURL::builtin(BuiltinIcon::CheckCircle).setFill(SemanticColor::Green)
                    : ImageURL::builtin(BuiltinIcon::Circle);
    return {ListAccessory{.icon = icon}};
  }

  std::unique_ptr<ActionPanelState> buildActionPanel(const ItemType &row) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    const bool toggle = row.entry.toggleType != TrayMenuItem::ToggleType::None;
    panel->createSection()->addAction(
        new StaticAction(tr("Trigger"), ImageURL::builtin(BuiltinIcon::Bolt),
                         [item = m_item, id = row.entry.id, toggle](ApplicationContext *ctx) {
                           ctx->services->trayHost()->triggerMenuItem(item, id);
                           if (!toggle)
                             ctx->navigation->closeWindow({.popToRootType = PopToRootType::Immediate});
                         }));
    return panel;
  }

private:
  void reload() {
    setLoading(true);
    context()->services->trayHost()->menu(m_item).then(this, [this](std::vector<TrayMenuItem> entries) {
      std::vector<TrayMenuRow> rows;
      flatten(entries, {}, rows);
      setItems(std::move(rows));
      setLoading(false);
    });
  }

  static void flatten(const std::vector<TrayMenuItem> &entries, const QString &prefix,
                      std::vector<TrayMenuRow> &out) {
    for (const auto &e : entries) {
      if (!e.visible || e.separator) continue;
      const auto label = prefix.isEmpty() ? e.plainLabel() : prefix + " › " + e.plainLabel();
      if (e.submenu) {
        flatten(e.children, label, out);
        continue;
      }
      if (!e.enabled || label.isEmpty()) continue;
      out.push_back({.entry = e, .label = label});
    }
  }

  TrayItem m_item;
};

class SearchTrayViewHost : public MonoListViewHost<TrayItem> {
  Q_DECLARE_TR_FUNCTIONS(SearchTrayViewHost)

public:
  void onMount() override {
    setSearchPlaceholderText(tr("Search tray items..."));
    connect(context()->services->trayHost(), &AbstractTrayHost::itemsChanged, this, [this]() { reload(); });
    reload();
  }

  QString displayTitle(const ItemType &item) const override { return trayItemTitle(item); }
  QString displayId(const ItemType &item) const override { return item.key(); }
  std::optional<ImageURL> displayIcon(const ItemType &item) const override { return item.icon(); }

  QString displaySubtitle(const ItemType &item) const override {
    if (!item.tooltipTitle.isEmpty() && item.tooltipTitle != trayItemTitle(item)) return item.tooltipTitle;
    return item.tooltipDescription;
  }

  AccessoryList displayAccessories(const ItemType &item) const override {
    if (item.status != TrayItem::Status::NeedsAttention) return {};
    return {ListAccessory{.text = tr("Attention"), .color = SemanticColor::Orange}};
  }

  std::unique_ptr<ActionPanelState> buildActionPanel(const ItemType &item) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto main = panel->createSection();

    auto browse = [item]() {
      return new StaticAction(
          tr("Browse Menu"), ImageURL::builtin(BuiltinIcon::BulletPoints),
          [item](ApplicationContext *ctx) { ctx->navigation->pushView(new TrayMenuViewHost(item)); });
    };
    auto activate = [item]() {
      return new StaticAction(tr("Activate"), ImageURL::builtin(BuiltinIcon::Bolt),
                              [item](ApplicationContext *ctx) {
                                ctx->services->trayHost()->activate(item);
                                ctx->navigation->closeWindow({.popToRootType = PopToRootType::Immediate});
                              });
    };

    if (item.itemIsMenu) {
      if (item.hasMenu()) main->addAction(browse());
      return panel;
    }

    main->addAction(activate());
    if (item.hasMenu()) main->addAction(browse());
    main->addAction(new StaticAction(
        tr("Secondary Activate"), ImageURL::builtin(BuiltinIcon::Bolt), [item](ApplicationContext *ctx) {
          ctx->services->trayHost()->secondaryActivate(item);
          ctx->navigation->closeWindow({.popToRootType = PopToRootType::Immediate});
        }));
    return panel;
  }

private:
  void reload() { setItems(context()->services->trayHost()->items()); }
};
