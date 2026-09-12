#pragma once
#include <optional>
#include <QString>
#include <QVariantList>
#include "ui/views/detail-list-view-host.hpp"
#include "ui/views/fuzzy-section.hpp"
#include "ui/views/view-utils.hpp"

struct ListItemDetail {
  QVariantList metadata;
  QString markdown;
};

/**
 * A list view with a single section.
 * Does not require creating a separate model, much simpler way to create new views.
 * Override `hasDetailPane` and `displayDetail` to show a metadata + markdown panel for the selected item.
 */
template <typename T> class MonoListViewHost : public DetailListViewHost, public FuzzySection<T> {

public:
  using ItemType = T;

  virtual void onMount() {}

  bool hasDetail() const final { return m_detail.has_value(); }
  QString detailContent() const final { return m_detail ? m_detail->markdown : QString(); }
  QVariantList detailMetadata() const final { return m_detail ? m_detail->metadata : QVariantList(); }
  bool detailMarkdown() const final { return true; }

protected:
  QString sectionName() const override { return ""; }

  QString displayTitle(const T &e) const override = 0;
  QString displaySubtitle(const T &e) const override = 0;
  std::optional<ImageURL> displayIcon(const T &e) const override = 0;
  AccessoryList displayAccessories(const T &e) const override = 0;
  std::unique_ptr<ActionPanelState> buildActionPanel(const T &e) const override = 0;

  virtual bool hasDetailPane() const { return false; }
  virtual std::optional<ListItemDetail> displayDetail(const T &) const { return std::nullopt; }

  void refreshDetail() {
    if (!hasDetailPane()) return;
    const bool valid = m_selected && *m_selected >= 0 && *m_selected < this->count();
    m_detail = valid ? displayDetail(this->at(*m_selected)) : std::nullopt;
    emit detailChanged();
  }

private:
  QUrl qmlComponentUrl() const final {
    return hasDetailPane() ? qml::componentUrl(u"DetailListView") : listModel()->qmlComponentUrl();
  }

  QVariantMap qmlProperties() final {
    if (hasDetailPane()) {
      return {{QStringLiteral("host"), QVariant::fromValue(static_cast<DetailListViewHost *>(this))}};
    }
    return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(listModel()))}};
  }

  void initialize() final {
    BaseView::initialize();
    initModel();
    model()->addSource(this);

    connect(model(), &SectionListModel::itemSelected, this, [this](SectionSource *source, int itemIdx) {
      if (source != this) return;
      m_selected = itemIdx;
      refreshDetail();
    });
    connect(model(), &SectionListModel::selectionCleared, this, [this]() {
      m_selected.reset();
      refreshDetail();
    });

    onMount();
  }

  std::optional<int> m_selected;
  std::optional<ListItemDetail> m_detail;
};
