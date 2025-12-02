#pragma once
#include "common.hpp"
#include "layout.hpp"
#include "services/clipboard/clipboard-db.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "ui/vlist/common/section-model.hpp"
#include "ui/vlist/vlist.hpp"
#include "utils.hpp"

class ClipboardHistoryItemWidget : public SelectableOmniListWidget {
public:
  void setEntry(const ClipboardHistoryEntry &entry) {
    auto createdAt = QDateTime::fromSecsSinceEpoch(entry.updatedAt);
    m_title->setText(entry.textPreview);
    m_pinIcon->setVisible(entry.pinnedAt);
    m_description->setText(QString("%1").arg(getRelativeTimeString(createdAt)));
    m_icon->setFixedSize(25, 25);
    m_icon->setUrl(iconForMime(entry));
  }

  ClipboardHistoryItemWidget() { setupUI(); }

private:
  TypographyWidget *m_title = new TypographyWidget;
  TypographyWidget *m_description = new TypographyWidget;
  ImageWidget *m_icon = new ImageWidget;
  ImageWidget *m_pinIcon = new ImageWidget;

  ImageURL getLinkIcon(const std::optional<QString> &urlHost) const {
    auto dflt = ImageURL::builtin("link");

    if (urlHost) return ImageURL::favicon(*urlHost).withFallback(dflt);

    return dflt;
  }

  ImageURL iconForMime(const ClipboardHistoryEntry &entry) const {
    switch (entry.kind) {
    case ClipboardOfferKind::Image:
      return ImageURL::builtin("image");
    case ClipboardOfferKind::Link:
      return getLinkIcon(entry.urlHost);
    case ClipboardOfferKind::Text:
      return ImageURL::builtin("text");
    case ClipboardOfferKind::File:
      return ImageURL::builtin("folder");
    default:
      break;
    }
    return ImageURL::builtin("question-mark-circle");
  }

  void setupUI() {
    m_pinIcon->setUrl(ImageURL::builtin("pin").setFill(SemanticColor::Red));
    m_pinIcon->setFixedSize(16, 16);
    m_description->setColor(SemanticColor::TextMuted);
    m_description->setSize(TextSize::TextSmaller);

    auto layout = HStack().margins(5).spacing(10).add(m_icon).add(
        VStack().add(m_title).add(HStack().add(m_pinIcon).add(m_description).spacing(5)));

    setLayout(layout.buildLayout());
  }
};

enum class ClipboardHistorySection { Main };

class ClipboardHistoryModel
    : public vicinae::ui::SectionListModel<const ClipboardHistoryEntry *, ClipboardHistorySection> {
  Q_OBJECT

public:
  ClipboardHistoryModel(QObject *parent = nullptr) { setParent(parent); }

  void setData(const PaginatedResponse<ClipboardHistoryEntry> &data) {
    m_res = data;
    emit dataChanged();
  }

protected:
  int sectionCount() const override { return 1; }
  ClipboardHistorySection sectionIdFromIndex(int idx) const override { return ClipboardHistorySection::Main; }
  int sectionItemCount(ClipboardHistorySection id) const override { return m_res.data.size(); }
  std::string_view sectionName(ClipboardHistorySection id) const override { return ""; }
  const ClipboardHistoryEntry *sectionItemAt(ClipboardHistorySection id, int itemIdx) const override {
    return &m_res.data[itemIdx];
  }
  StableID stableId(const ClipboardHistoryEntry *const &item) const override {
    static std::hash<QString> hasher = {};
    return hasher(item->id);
  }
  int sectionItemHeight(ClipboardHistorySection id) const override { return 50; }

  WidgetTag widgetTag(const ClipboardHistoryEntry *const &item) const override { return 1; }

  WidgetType *createItemWidget(const ClipboardHistoryEntry *const &type) const override {
    return new ClipboardHistoryItemWidget;
  }

  void refreshItemWidget(const ClipboardHistoryEntry *const &entry, WidgetType *widget) const override {
    static_cast<ClipboardHistoryItemWidget *>(widget)->setEntry(*entry);
  }

private:
  PaginatedResponse<ClipboardHistoryEntry> m_res;
};
