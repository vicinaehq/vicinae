#pragma once
#include "extend/list-model.hpp"
#include "extension/extension-action-panel-builder.hpp"
#include "qml-command-list-model.hpp"
#include <functional>

class QmlExtensionListModel : public QmlCommandListModel {
  Q_OBJECT

public:
  using NotifyFn = std::function<void(const QString &handler, const QJsonArray &args)>;

  explicit QmlExtensionListModel(NotifyFn notify, QObject *parent = nullptr);

  void setExtensionData(const ListModel &model);

  void setFilter(const QString &text) override;
  QString searchPlaceholder() const override;
  QUrl qmlComponentUrl() const override { return QUrl(QStringLiteral("qrc:/qml/CommandListView.qml")); }
  void onItemSelected(int section, int item) override;

protected:
  QString itemTitle(int section, int item) const override;
  QString itemSubtitle(int section, int item) const override;
  QString itemIconSource(int section, int item) const override;
  QString itemAccessory(int section, int item) const override;
  std::unique_ptr<ActionPanelState> createActionPanel(int section, int item) const override;

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
  std::vector<Section> m_sections;
  std::vector<Section> m_filteredSections; // Used when client-side filtering is active
  ListModel m_model;
  QString m_filter;
  QString m_placeholder;

  const std::vector<Section> &activeSections() const;
};
