#pragma once
#include "builtin_icon.hpp"
#include "common/context.hpp"
#include "fuzzy-section.hpp"
#include "list-view-host.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/status-notifier/system-tray-service.hpp"
#include "theme/colors.hpp"
#include "ui/action-pannel/action-panel-state.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/list-accessory/list-accessory.hpp"
#include <QBuffer>
#include <QCoreApplication>
#include <QFuture>
#include <memory>

struct TrayRow {
  TrayItem item;
  std::optional<TrayMenuItem> menu;
  QString label;
};

template <> struct fuzzy::FuzzySearchable<TrayRow> {
  static fuzzy::Match score(const TrayRow &row, const fuzzy::Query &query) {
    const auto title = row.item.title.isEmpty() ? row.item.id : row.item.title;
    return fuzzy::scoreWeighted({{row.label.toStdString(), 1.0}, {title.toStdString(), 0.5}}, query);
  }
};

class TrayItemSection : public FuzzySection<TrayRow> {
  Q_DECLARE_TR_FUNCTIONS(TrayItemSection)

public:
  explicit TrayItemSection(const TrayEntry &entry) : m_name(itemTitle(entry.item)) {
    std::vector<TrayRow> rows;

    if (!entry.item.itemIsMenu || entry.menu.empty()) {
      rows.push_back({.item = entry.item, .label = m_name});
    }
    flatten(entry.item, entry.menu, {}, rows);
    setItems(std::move(rows));
  }

  QString sectionName() const override { return m_name; }

protected:
  QString displayTitle(const TrayRow &row) const override { return row.label; }
  QString displayId(const TrayRow &row) const override {
    return row.item.key() + (row.menu ? "/" + QString::number(row.menu->id) : QString());
  }

  QString displaySubtitle(const TrayRow &row) const override {
    if (row.menu) return {};
    if (!row.item.tooltipTitle.isEmpty() && row.item.tooltipTitle != row.label) return row.item.tooltipTitle;
    return row.item.tooltipDescription;
  }

  std::optional<ImageURL> displayIcon(const TrayRow &row) const override {
    if (!row.menu) return row.item.icon();
    if (!row.menu->iconData.isNull()) {
      QByteArray bytes;
      QBuffer buf(&bytes);
      buf.open(QIODevice::WriteOnly);
      row.menu->iconData.save(&buf, "PNG");
      return ImageURL::rawData(bytes, "image/png");
    }
    if (!row.menu->iconName.isEmpty()) return ImageURL::system(row.menu->iconName);
    return row.item.icon();
  }

  AccessoryList displayAccessories(const TrayRow &row) const override {
    if (!row.menu) {
      if (row.item.status == TrayItem::Status::NeedsAttention) {
        return {ListAccessory{.text = tr("Attention"), .color = SemanticColor::Orange}};
      }
      return {};
    }
    if (row.menu->toggleType != TrayMenuItem::ToggleType::None) {
      auto icon = row.menu->toggleState == 1
                      ? ImageURL::builtin(BuiltinIcon::CheckCircle).setFill(SemanticColor::Green)
                      : ImageURL::builtin(BuiltinIcon::Circle);
      return {ListAccessory{.icon = icon}};
    }
    return {};
  }

  std::unique_ptr<ActionPanelState> buildActionPanel(const TrayRow &row) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto main = panel->createSection();

    if (row.menu) {
      main->addAction(new StaticAction(tr("Trigger"), ImageURL::builtin(BuiltinIcon::Bolt),
                                       [item = row.item, id = row.menu->id](ApplicationContext *ctx) {
                                         tray(ctx)->triggerMenuItem(item, id);
                                         ctx->navigation->closeWindow();
                                       }));
      return panel;
    }

    main->addAction(new StaticAction(tr("Activate"), ImageURL::builtin(BuiltinIcon::Bolt),
                                     [item = row.item](ApplicationContext *ctx) {
                                       tray(ctx)->activate(item);
                                       ctx->navigation->closeWindow();
                                     }));
    main->addAction(new StaticAction(tr("Secondary activate"), ImageURL::builtin(BuiltinIcon::Bolt),
                                     [item = row.item](ApplicationContext *ctx) {
                                       tray(ctx)->secondaryActivate(item);
                                       ctx->navigation->closeWindow();
                                     }));
    return panel;
  }

private:
  static AbstractTrayService *tray(ApplicationContext *ctx) { return ctx->services->systemTray()->provider(); }
  static QString itemTitle(const TrayItem &item) { return item.title.isEmpty() ? item.id : item.title; }

  static void flatten(const TrayItem &item, const std::vector<TrayMenuItem> &entries, const QString &prefix,
                      std::vector<TrayRow> &out) {
    for (const auto &e : entries) {
      if (!e.visible || e.separator) continue;
      const auto label = prefix.isEmpty() ? e.plainLabel() : prefix + " › " + e.plainLabel();
      if (e.submenu) {
        flatten(item, e.children, label, out);
        continue;
      }
      if (!e.enabled || label.isEmpty()) continue;
      out.push_back({.item = item, .menu = e, .label = label});
    }
  }

  QString m_name;
};

class SearchTrayViewHost : public ListViewHost {
  Q_DECLARE_TR_FUNCTIONS(SearchTrayViewHost)

public:
  void initialize() override {
    BaseView::initialize();
    initModel();
    setSearchPlaceholderText(tr("Search tray items and menus..."));
    auto *svc = context()->services->systemTray()->provider();
    connect(svc, &AbstractTrayService::changed, this, [this]() { reload(); });
  }

  void loadInitialData() override { reload(); }

  void textChanged(const QString &text) override {
    m_filter = text;
    ListViewHost::textChanged(text);
  }

private:
  void reload() {
    auto *svc = context()->services->systemTray()->provider();
    svc->snapshot().then(this, [this](std::vector<TrayEntry> entries) {
      model()->clearSources();
      m_sections.clear();
      for (const auto &entry : entries) {
        m_sections.push_back(std::make_unique<TrayItemSection>(entry));
        model()->addSource(m_sections.back().get());
      }
      model()->setFilter(m_filter);
    });
  }

  std::vector<std::unique_ptr<TrayItemSection>> m_sections;
  QString m_filter;
};
