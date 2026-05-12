#pragma once
#include "bridge-view.hpp"
#include "section-list-model.hpp"

class ListViewHost : public ViewHostBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

public:
  QUrl qmlComponentUrl() const override { return m_model.qmlComponentUrl(); }

  QVariantMap qmlProperties() override {
    return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(&m_model))}};
  }

  void textChanged(const QString &text) override { m_model.setFilter(text); }
  void onReactivated() override { m_model.refreshActionPanel(); }
  void beforePop() override { m_model.beforePop(); }

  QObject *listModel() const { return const_cast<SectionListModel *>(&m_model); }

protected:
  SectionListModel *model() { return &m_model; }

  void initModel() {
    m_model.setScope(ViewScope(context(), this));

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

private:
  SectionListModel m_model{this};
};
