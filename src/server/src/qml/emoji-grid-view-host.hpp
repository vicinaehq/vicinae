#pragma once
#include "bridge-view.hpp"
#include "emoji-grid-model.hpp"

class EmojiGridViewHost : public ViewHostBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

public:
  QUrl qmlComponentUrl() const override { return QUrl(QStringLiteral("qrc:/Vicinae/EmojiGridView.qml")); }

  QVariantMap qmlProperties() override {
    return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(&m_model))}};
  }

  void initialize() override {
    BaseView::initialize();
    m_model.setScope(ViewScope(context(), this));
    m_model.initialize();
    setSearchPlaceholderText(m_model.searchPlaceholder());
  }

  void textChanged(const QString &text) override { m_model.setFilter(text); }
  void beforePop() override { m_model.beforePop(); }
  void onReactivated() override { m_model.refreshActionPanel(); }
  void loadInitialData() override { m_model.setFilter(searchText()); }

  QObject *listModel() const { return const_cast<EmojiGridModel *>(&m_model); }

private:
  EmojiGridModel m_model{this};
};
