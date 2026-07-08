#pragma once
#include "extend/grid-model.hpp"
#include "image-url.hpp"
#include "extension/extension-action-panel-builder.hpp"
#include "fuzzy/scored.hpp"
#include "grid-source.hpp"
#include "section-grid-model.hpp"
#include <functional>
#include <memory>
#include <vector>

class ExtensionGridSection : public GridSource {
public:
  using NotifyFn = ExtensionActionPanelBuilder::NotifyFn;

  ExtensionGridSection(std::string name, std::vector<GridItemViewModel> items, std::optional<int> columns,
                       std::optional<double> aspectRatio, std::optional<GridInset> inset, bool filtering,
                       NotifyFn notify, const std::optional<ActionPannelModel> *globalActions);

  QString sectionName() const override { return QString::fromStdString(m_name); }
  int count() const override;
  std::optional<int> columns() const override { return m_columns; }
  std::optional<double> aspectRatio() const override { return m_aspectRatio; }
  std::optional<double> inset() const override;
  void setFilter(std::string_view query) override;

  void setOnItemSelected(std::function<void(const GridItemViewModel *)> cb) {
    m_onItemSelected = std::move(cb);
  }
  void onSelected(int i) override;

  const GridItemViewModel *itemAt(int i) const;

  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  std::string m_name;
  std::vector<GridItemViewModel> m_items;
  std::vector<Scored<int>> m_filtered;
  std::optional<int> m_columns;
  std::optional<double> m_aspectRatio;
  std::optional<GridInset> m_inset;
  bool m_filtering;
  std::string m_query;
  NotifyFn m_notify;
  const std::optional<ActionPannelModel> *m_globalActions;
  std::function<void(const GridItemViewModel *)> m_onItemSelected;
};

class ExtensionGridModel : public SectionGridModel {
  Q_OBJECT
  Q_PROPERTY(QString emptyTitle READ emptyTitle NOTIFY emptyViewChanged)
  Q_PROPERTY(QString emptyDescription READ emptyDescription NOTIFY emptyViewChanged)
  Q_PROPERTY(ImageUrl emptyIcon READ emptyIcon NOTIFY emptyViewChanged)
  Q_PROPERTY(int fit READ fit NOTIFY fitChanged)
  Q_PROPERTY(int dataRevision READ dataRevision NOTIFY dataRevisionChanged)

public:
  using NotifyFn = std::function<void(const QString &handler, const QJsonArray &args)>;

  explicit ExtensionGridModel(NotifyFn notify, QObject *parent = nullptr);

  void setExtensionData(const GridModel &model, bool resetSelection = true);
  void setFilter(const QString &text);
  QString searchPlaceholder() const;
  QUrl qmlComponentUrl() const { return QUrl(QStringLiteral("qrc:/Vicinae/ExtensionGridView.qml")); }

  Q_INVOKABLE QString cellTitle(int section, int item) const;
  Q_INVOKABLE QString cellIcon(int section, int item) const;
  Q_INVOKABLE QString cellSubtitle(int section, int item) const;
  Q_INVOKABLE QString cellTooltip(int section, int item) const;
  Q_INVOKABLE QString cellColor(int section, int item) const;

  QString emptyTitle() const;
  QString emptyDescription() const;
  ImageUrl emptyIcon() const;
  int fit() const { return static_cast<int>(m_fit); }
  int dataRevision() const { return m_dataRevision; }

signals:
  void emptyViewChanged();
  void fitChanged();
  void dataRevisionChanged();

protected:
  void onSelectionCleared() override;

private:
  const GridItemViewModel *resolveItem(int section, int item) const;
  void rebuildFromSections(bool resetSelection);

  NotifyFn m_notify;
  std::vector<std::unique_ptr<ExtensionGridSection>> m_ownedSections;
  GridModel m_model;
  ObjectFit m_fit = ObjectFit::Contain;
  QString m_filter;
  QString m_placeholder;
  int m_dataRevision = 0;
};
