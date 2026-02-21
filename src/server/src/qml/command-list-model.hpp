#pragma once
#include "common/context.hpp"
#include "view-utils.hpp"
#include "theme.hpp"
#include <QAbstractListModel>
#include <QUrl>
#include <memory>
#include <vector>

class ActionPanelState;

class CommandListModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(bool selectFirstOnReset READ selectFirstOnReset NOTIFY selectFirstOnResetChanged)

signals:
  void selectFirstOnResetChanged();

public:
  enum Role {
    IsSection = Qt::UserRole + 1,
    IsSelectable,
    SectionName,
    Title,
    Subtitle,
    IconSource,
    Accessory,
  };

  explicit CommandListModel(QObject *parent = nullptr) : QAbstractListModel(parent) {
    connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() {
      if (rowCount() > 0) emit dataChanged(index(0), index(rowCount() - 1), {IconSource});
    });
  }

  virtual void initialize(ApplicationContext *ctx);

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  bool selectFirstOnReset() const { return m_selectFirstOnReset; }

  Q_INVOKABLE void setSelectedIndex(int index);
  Q_INVOKABLE void activateSelected();
  Q_INVOKABLE int nextSelectableIndex(int from, int direction) const;

  void refreshActionPanel();

  virtual void setFilter(const QString &text) = 0;
  virtual QString searchPlaceholder() const { return QStringLiteral("Search for anything..."); }
  virtual QUrl qmlComponentUrl() const { return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml")); }
  virtual void onItemSelected(int section, int item) {}
  virtual void beforePop() {}

protected:
  virtual QString itemTitle(int section, int item) const = 0;
  virtual QString itemSubtitle(int section, int item) const { return {}; }
  virtual QString itemIconSource(int section, int item) const = 0;
  virtual QVariantList itemAccessory(int section, int item) const { return {}; }
  virtual QString itemId(int section, int item) const { return itemTitle(section, item); }
  virtual std::unique_ptr<ActionPanelState> createActionPanel(int section, int item) const = 0;
  virtual void onSelectionCleared();

  void setSelectFirstOnReset(bool value) {
    if (m_selectFirstOnReset != value) {
      m_selectFirstOnReset = value;
      emit selectFirstOnResetChanged();
    }
  }

  struct SectionInfo {
    QString name;
    int count;
  };
  void setSections(const std::vector<SectionInfo> &sections);

  void invalidateSelection() { m_lastSelectedItemId.clear(); }

  ApplicationContext *ctx() const { return m_ctx; }
  QString imageSourceFor(const ImageURL &url) const { return qml::imageSourceFor(url); }

  // Returns true if row is a data item, filling section and item indices.
  // Returns false if row is invalid or a section header.
  bool dataItemAt(int row, int &section, int &item) const;

private:
  struct FlatItem {
    enum Kind { SectionHeader, DataItem } kind;
    int sectionIdx;
    int itemIdx;
  };

  static std::vector<FlatItem> buildFlatList(const std::vector<SectionInfo> &sections);

  ApplicationContext *m_ctx = nullptr;
  std::vector<SectionInfo> m_sectionInfos;
  std::vector<FlatItem> m_flat;
  int m_selectedIndex = -1;
  QString m_lastSelectedItemId;
  bool m_selectFirstOnReset = true;
};
