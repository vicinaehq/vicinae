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

  void initModel() { m_model.setScope(ViewScope(context(), this)); }

private:
  SectionListModel m_model{this};
};
