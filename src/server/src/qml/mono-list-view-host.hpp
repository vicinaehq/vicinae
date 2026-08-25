#pragma once
#include "bridge-view.hpp"
#include "fuzzy-section.hpp"
#include "section-list-model.hpp"

/**
 * A list view with a single section.
 * Does not require creating a separate model, much simpler way to create new views.
 */
template <typename T> class MonoListViewHost : public ViewHostBase, public FuzzySection<T> {
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

public:
  using ItemType = T;

  virtual void onMount() { initialize(); }

protected:
  QString sectionName() const override { return ""; }

  QString displayTitle(const T &e) const override = 0;
  QString displaySubtitle(const T &e) const override = 0;
  std::optional<ImageURL> displayIcon(const T &e) const override = 0;
  AccessoryList displayAccessories(const T &e) const override = 0;
  std::unique_ptr<ActionPanelState> buildActionPanel(const T &e) const override = 0;

  void textChanged(const QString &text) override { m_model.setFilter(text); }
  void onReactivated() override { m_model.refreshActionPanel(); }
  void beforePop() override { m_model.beforePop(); }

  SectionListModel m_model{this};
  SectionListModel *quickAccessModel() override { return &m_model; }

private:
  QUrl qmlComponentUrl() const final { return m_model.qmlComponentUrl(); }

  QVariantMap qmlProperties() final {
    return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(&m_model))}};
  }

  void initialize() final {
    BaseView::initialize();
    initModel();
    onMount();
  }

  QObject *listModel() const { return const_cast<SectionListModel *>(&m_model); }

  SectionListModel *model() { return &m_model; }

  void initModel() {
    m_model.setScope(ViewScope(context(), this));
    m_model.addSource(this);

    connect(&m_model, &SectionListModel::itemSelected, this, [this](SectionSource *source, int itemIdx) {
      if (auto panel = source->actionPanel(itemIdx))
        setActions(std::move(panel));
      else
        clearActions();
    });

    connect(&m_model, &SectionListModel::selectionCleared, this, [this]() {
      if (auto panel = emptyActionPanel())
        setActions(std::move(panel));
      else
        clearActions();
    });
  }

  virtual std::unique_ptr<ActionPanelState> emptyActionPanel() { return nullptr; }
};
