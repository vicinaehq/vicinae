#pragma once
#include "common/paginated.hpp"
#include "section-source.hpp"
#include "services/clipboard/clipboard-db.hpp"
#include <functional>

class ClipboardHistorySection : public SectionSource {
public:
  enum ExtraRole { IsPinned = 100 };
  enum class DefaultAction { Copy, Paste };

  void setEntries(const PaginatedResponse<ClipboardHistoryEntry> &page);
  void setDefaultAction(DefaultAction action) { m_defaultAction = action; }

  QString sectionName() const override { return {}; }
  int count() const override { return static_cast<int>(m_entries.size()); }

  const ClipboardHistoryEntry &entryAt(int i) const { return m_entries[i]; }

  void setOnEntrySelected(std::function<void(const ClipboardHistoryEntry &)> cb) {
    m_onEntrySelected = std::move(cb);
  }

  void onSelected(int i) override {
    if (i >= 0 && std::cmp_less(i, m_entries.size()) && m_onEntrySelected) {
      m_onEntrySelected(m_entries[i]);
    }
  }

  QVariant customData(int i, int role) const override {
    if (role == IsPinned) return m_entries[i].pinnedAt != 0;
    return {};
  }

  QHash<int, QByteArray> customRoleNames() const override { return {{IsPinned, "isPinned"}}; }

protected:
  QString itemId(int i) const override;
  QString itemTitle(int i) const override;
  QString itemSubtitle(int i) const override;
  QString itemIconSource(int i) const override;
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  ImageURL iconForEntry(const ClipboardHistoryEntry &entry) const;

  std::vector<ClipboardHistoryEntry> m_entries;
  DefaultAction m_defaultAction = DefaultAction::Copy;
  std::function<void(const ClipboardHistoryEntry &)> m_onEntrySelected;
};
