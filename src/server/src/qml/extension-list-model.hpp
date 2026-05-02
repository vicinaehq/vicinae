#pragma once
#include "extend/list-model.hpp"
#include "extension/extension-action-panel-builder.hpp"
#include "fuzzy/scored.hpp"
#include "section-list-model.hpp"
#include "section-source.hpp"
#include <functional>
#include <memory>
#include <vector>

class ExtensionListSection : public SectionSource {
public:
  using NotifyFn = ExtensionActionPanelBuilder::NotifyFn;
  using SubmenuCache = ExtensionActionPanelBuilder::SubmenuCache;

  ExtensionListSection(std::string name, std::vector<ListItemViewModel> items, bool filtering,
                       NotifyFn notify, SubmenuCache *cache,
                       const std::optional<ActionPannelModel> *globalActions);

  QString sectionName() const override { return QString::fromStdString(m_name); }
  int count() const override;
  void setFilter(std::string_view query) override;

  void setOnItemSelected(std::function<void(const ListItemViewModel *)> cb) {
    m_onItemSelected = std::move(cb);
  }
  void onSelected(int i) override;

protected:
  QString itemTitle(int i) const override;
  QString itemSubtitle(int i) const override;
  QString itemIconSource(int i) const override;
  QVariantList itemAccessories(int i) const override;
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  const ListItemViewModel &itemAt(int i) const;

  std::string m_name;
  std::vector<ListItemViewModel> m_items;
  std::vector<Scored<int>> m_filtered;
  bool m_filtering;
  std::string m_query;
  NotifyFn m_notify;
  SubmenuCache *m_cache;
  const std::optional<ActionPannelModel> *m_globalActions;
  std::function<void(const ListItemViewModel *)> m_onItemSelected;
};

class ExtensionListModel : public SectionListModel {
  Q_OBJECT
  Q_PROPERTY(QString emptyTitle READ emptyTitle NOTIFY emptyViewChanged)
  Q_PROPERTY(QString emptyDescription READ emptyDescription NOTIFY emptyViewChanged)
  Q_PROPERTY(QString emptyIcon READ emptyIcon NOTIFY emptyViewChanged)
  Q_PROPERTY(bool isShowingDetail READ isShowingDetail NOTIFY detailChanged)
  Q_PROPERTY(bool hasDetail READ hasDetail NOTIFY detailChanged)
  Q_PROPERTY(QString detailMarkdown READ detailMarkdown NOTIFY detailChanged)
  Q_PROPERTY(QVariantList detailMetadata READ detailMetadata NOTIFY detailChanged)

public:
  using NotifyFn = ExtensionActionPanelBuilder::NotifyFn;

  explicit ExtensionListModel(NotifyFn notify, QObject *parent = nullptr);

  void setExtensionData(const ListModel &model, bool resetSelection);
  void setFilter(const QString &text);

  QString searchPlaceholder() const;

  QString emptyTitle() const;
  QString emptyDescription() const;
  QString emptyIcon() const;

  bool isShowingDetail() const;
  bool hasDetail() const;
  QString detailMarkdown() const;
  QVariantList detailMetadata() const;

signals:
  void detailChanged();
  void emptyViewChanged();

protected:
  void onSelectionCleared() override;

private:
  void handleItemSelected(const ListItemViewModel *item);

  NotifyFn m_notify;
  mutable ExtensionActionPanelBuilder::SubmenuCache m_submenuCache;
  std::optional<DetailModel> m_currentDetail;
  std::vector<std::unique_ptr<ExtensionListSection>> m_ownedSections;
  ListModel m_model;
  QString m_filter;
  QString m_placeholder;
};
