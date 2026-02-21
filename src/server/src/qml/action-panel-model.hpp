#pragma once
#include "view-utils.hpp"
#include "navigation-controller.hpp"
#include <QAbstractListModel>
#include <memory>
#include <vector>

class ActionPanelModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(QString title READ title NOTIFY titleChanged)

public:
  enum Role {
    ItemType = Qt::UserRole + 1, // "section", "action", "divider"
    Title,
    IconSource,
    ShortcutLabel,
    IsSubmenu,
    IsPrimary,
    IsDanger,
  };

  explicit ActionPanelModel(QObject *parent = nullptr);
  explicit ActionPanelModel(std::unique_ptr<ActionPanelState> state, QObject *parent = nullptr);
  explicit ActionPanelModel(const ActionPanelState *state, QObject *parent = nullptr);

  void setState(std::unique_ptr<ActionPanelState> state);
  void setStateFrom(const ActionPanelState *state);
  QString title() const { return m_title; }

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  Q_INVOKABLE void activate(int index);
  Q_INVOKABLE void setFilter(const QString &text);
  Q_INVOKABLE int nextSelectableIndex(int from, int direction) const;

  bool activateByShortcut(int key, int modifiers);

signals:
  void titleChanged();
  void actionExecuted(AbstractAction *action);
  void submenuRequested(ActionPanelModel *subModel);
  void customPanelRequested(const QUrl &componentUrl, const QVariantMap &properties);
  void closeRequested();

private:
  struct FlatItem {
    enum Kind { SectionHeader, ActionItem, Divider } kind;
    int sectionIdx = -1;
    int actionIdx = -1;
  };

  void rebuildFlatList();
  bool matchesFilter(const QString &title) const;

  QString m_title;
  QString m_filter;
  QStringList m_filterWords;

  // Owns all actions so they outlive the state
  std::vector<std::shared_ptr<AbstractAction>> m_allActions;
  struct SectionInfo {
    QString name;
    std::vector<std::shared_ptr<AbstractAction>> actions;
  };
  std::vector<SectionInfo> m_sections;
  std::vector<FlatItem> m_flat;
};
