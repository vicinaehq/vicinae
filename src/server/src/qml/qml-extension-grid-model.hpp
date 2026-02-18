#pragma once
#include "extend/grid-model.hpp"
#include "extension/extension-action-panel-builder.hpp"
#include "qml-command-grid-model.hpp"
#include <functional>

class QmlExtensionGridModel : public QmlCommandGridModel {
  Q_OBJECT
  Q_PROPERTY(QString emptyTitle READ emptyTitle NOTIFY emptyViewChanged)
  Q_PROPERTY(QString emptyDescription READ emptyDescription NOTIFY emptyViewChanged)
  Q_PROPERTY(QString emptyIcon READ emptyIcon NOTIFY emptyViewChanged)
  Q_PROPERTY(int fit READ fit NOTIFY fitChanged)
  Q_PROPERTY(double inset READ inset NOTIFY insetChanged)

public:
  using NotifyFn = std::function<void(const QString &handler, const QJsonArray &args)>;

  explicit QmlExtensionGridModel(NotifyFn notify, QObject *parent = nullptr);

  void setExtensionData(const GridModel &model);

  void setFilter(const QString &text) override;
  QString searchPlaceholder() const override;
  QUrl qmlComponentUrl() const override { return QUrl(QStringLiteral("qrc:/Vicinae/ExtensionGridView.qml")); }

  // Per-cell data for QML delegate
  Q_INVOKABLE QString cellTitle(int section, int item) const;
  Q_INVOKABLE QString cellIcon(int section, int item) const;
  Q_INVOKABLE QString cellSubtitle(int section, int item) const;
  Q_INVOKABLE QString cellTooltip(int section, int item) const;

  QString emptyTitle() const;
  QString emptyDescription() const;
  QString emptyIcon() const;
  int fit() const { return static_cast<int>(m_fit); }
  double inset() const { return m_inset; }

signals:
  void emptyViewChanged();
  void fitChanged();
  void insetChanged();

protected:
  std::unique_ptr<ActionPanelState> createActionPanel(int section, int item) const override;
  void onItemSelected(int section, int item) override;
  void onSelectionCleared() override;

private:
  struct Section {
    std::string name;
    std::optional<double> aspectRatio;
    std::optional<int> columns;
    std::vector<GridItemViewModel> items;
  };

  void rebuildFromModel();
  bool matchesFilter(const GridItemViewModel &item, const QString &filter) const;
  const GridItemViewModel *itemAt(int section, int item) const;

  NotifyFn m_notify;
  mutable ExtensionActionPanelBuilder::SubmenuCache m_submenuCache;
  std::vector<Section> m_sections;
  std::vector<Section> m_filteredSections;
  GridModel m_model;
  ObjectFit m_fit = ObjectFit::Contain;
  double m_inset = 0.10;
  QString m_filter;
  QString m_placeholder;

  const std::vector<Section> &activeSections() const;
};
