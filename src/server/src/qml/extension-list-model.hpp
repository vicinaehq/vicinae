#pragma once
#include "extend/list-model.hpp"
#include "image-url.hpp"
#include "extension/extension-action-panel-builder.hpp"
#include "fuzzy/scored.hpp"
#include "section-list-model.hpp"
#include "section-source.hpp"
#include <QTimer>
#include <functional>
#include <memory>
#include <vector>

class ExtensionListSection : public SectionSource {
public:
  using NotifyFn = ExtensionActionPanelBuilder::NotifyFn;

  ExtensionListSection(std::string name, std::vector<ListItemViewModel> items, bool filtering,
                       NotifyFn notify, const std::optional<ActionPannelModel> *globalActions);

  QString sectionName() const override { return QString::fromStdString(m_name); }
  int count() const override;
  void setFilter(std::string_view query) override;

  void setOnItemSelected(std::function<void(const ListItemViewModel *)> cb) {
    m_onItemSelected = std::move(cb);
  }
  void onSelected(int i) override;

  const ListItemViewModel &itemAt(int i) const;

protected:
  QString itemId(int i) const override;
  QString itemTitle(int i) const override;
  QString itemSubtitle(int i) const override;
  QString itemIconSource(int i) const override;
  QVariantList itemAccessories(int i) const override;
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  std::string m_name;
  std::vector<ListItemViewModel> m_items;
  std::vector<Scored<int>> m_filtered;
  bool m_filtering;
  std::string m_query;
  NotifyFn m_notify;
  const std::optional<ActionPannelModel> *m_globalActions;
  std::function<void(const ListItemViewModel *)> m_onItemSelected;
};

class ExtensionListModel : public SectionListModel {
  Q_OBJECT
  Q_PROPERTY(QString emptyTitle READ emptyTitle NOTIFY emptyViewChanged)
  Q_PROPERTY(QString emptyDescription READ emptyDescription NOTIFY emptyViewChanged)
  Q_PROPERTY(ImageUrl emptyIcon READ emptyIcon NOTIFY emptyViewChanged)
  Q_PROPERTY(bool isShowingDetail READ isShowingDetail NOTIFY isShowingDetailChanged)
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
  ImageUrl emptyIcon() const;

  bool isShowingDetail() const;
  QString detailMarkdown() const;
  QVariantList detailMetadata() const { return m_detailMetadata; }

signals:
  void detailChanged();
  void isShowingDetailChanged();
  void emptyViewChanged();

protected:
  void onSelectionCleared() override;

private:
  void handleItemSelected(const ListItemViewModel *item);
  void refreshCurrentDetail();
  void setCurrentDetail(const DetailModel *detail);
  void scheduleDetailClear();

  NotifyFn m_notify;
  QTimer m_detailClearTimer;
  QString m_detailMarkdown;
  QVariantList m_detailMetadata;
  std::vector<std::unique_ptr<ExtensionListSection>> m_ownedSections;
  ListModel m_model;
  QString m_filter;
  QString m_placeholder;
};
