#pragma once
#include "extend/list-model.hpp"
#include "extension/extension-action-panel-builder.hpp"
#include "command-list-model.hpp"
#include <functional>

class ExtensionListModel : public CommandListModel {
  Q_OBJECT
  Q_PROPERTY(QString emptyTitle READ emptyTitle NOTIFY emptyViewChanged)
  Q_PROPERTY(QString emptyDescription READ emptyDescription NOTIFY emptyViewChanged)
  Q_PROPERTY(QString emptyIcon READ emptyIcon NOTIFY emptyViewChanged)
  Q_PROPERTY(bool isShowingDetail READ isShowingDetail NOTIFY detailChanged)
  Q_PROPERTY(bool hasDetail READ hasDetail NOTIFY detailChanged)
  Q_PROPERTY(QString detailMarkdown READ detailMarkdown NOTIFY detailChanged)
  Q_PROPERTY(QVariantList detailMetadata READ detailMetadata NOTIFY detailChanged)

public:
  using NotifyFn = std::function<void(const QString &handler, const QJsonArray &args)>;

  explicit ExtensionListModel(NotifyFn notify, QObject *parent = nullptr);

  void setExtensionData(const ListModel &model, bool resetSelection = true);

  void setFilter(const QString &text) override;
  QString searchPlaceholder() const override;
  QUrl qmlComponentUrl() const override { return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml")); }
  void onItemSelected(int section, int item) override;

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
  QString itemTitle(int section, int item) const override;
  QString itemSubtitle(int section, int item) const override;
  QString itemIconSource(int section, int item) const override;
  QVariant itemAccessory(int section, int item) const override;
  std::unique_ptr<ActionPanelState> createActionPanel(int section, int item) const override;
  void onSelectionCleared() override;

private:
  struct Section {
    std::string name;
    std::vector<ListItemViewModel> items;
  };

  void rebuildFromModel();
  bool matchesFilter(const ListItemViewModel &item, const QString &filter) const;
  const ListItemViewModel *itemAt(int section, int item) const;

  NotifyFn m_notify;
  mutable ExtensionActionPanelBuilder::SubmenuCache m_submenuCache;
  std::optional<DetailModel> m_currentDetail;
  std::vector<Section> m_sections;
  std::vector<Section> m_filteredSections; // Used when client-side filtering is active
  ListModel m_model;
  QString m_filter;
  QString m_placeholder;

  const std::vector<Section> &activeSections() const;
};
