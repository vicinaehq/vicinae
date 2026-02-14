#pragma once
#include "qml-command-list-model.hpp"
#include "common/paginated.hpp"
#include "services/clipboard/clipboard-db.hpp"

class QmlClipboardHistoryModel : public QmlCommandListModel {
  Q_OBJECT

public:
  enum ExtraRole {
    IsPinned = QmlCommandListModel::Accessory + 1,
  };

  enum class DefaultAction { Copy, Paste };

  explicit QmlClipboardHistoryModel(QObject *parent = nullptr);

  void setEntries(const PaginatedResponse<ClipboardHistoryEntry> &page);
  void setDefaultAction(DefaultAction action) { m_defaultAction = action; }
  void setFilter(const QString &text) override {}
  QString searchPlaceholder() const override { return QStringLiteral("Browse clipboard history..."); }

  QHash<int, QByteArray> roleNames() const override;
  QVariant data(const QModelIndex &index, int role) const override;

signals:
  void entrySelected(const ClipboardHistoryEntry &entry);

protected:
  QString itemTitle(int s, int i) const override;
  QString itemSubtitle(int s, int i) const override;
  QString itemIconSource(int s, int i) const override;
  std::unique_ptr<ActionPanelState> createActionPanel(int s, int i) const override;
  void onItemSelected(int s, int i) override;

private:
  ImageURL iconForEntry(const ClipboardHistoryEntry &entry) const;

  std::vector<ClipboardHistoryEntry> m_entries;
  DefaultAction m_defaultAction = DefaultAction::Copy;
};
