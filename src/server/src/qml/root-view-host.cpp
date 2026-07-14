#include "root-view-host.hpp"
#include "root-search-model.hpp"
#include "section-source.hpp"
#include "view-scope.hpp"

void RootViewHost::initialize() {
  BaseView::initialize();
  m_model = new RootSearchModel(ViewScope(context(), this), this);

  connect(m_model, &SectionListModel::itemSelected, this, [this](SectionSource *source, int itemIdx) {
    if (auto panel = source->actionPanel(itemIdx))
      setActions(std::move(panel));
    else
      clearActions();
  });

  connect(m_model, &SectionListModel::selectionCleared, this, [this]() { clearActions(); });

  m_model->setFilter({});
}

QUrl RootViewHost::qmlComponentUrl() const { return QUrl(QStringLiteral("qrc:/Vicinae/RootSearchList.qml")); }

QVariantMap RootViewHost::qmlProperties() {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void RootViewHost::textChanged(const QString &text) {
  if (m_model) m_model->setFilter(text);
}

void RootViewHost::onReactivated() {
  if (m_model) m_model->refreshActionPanel();
}

void RootViewHost::beforePop() {
  if (m_model) m_model->beforePop();
}

QObject *RootViewHost::listModel() const { return m_model; }

bool RootViewHost::tryAliasFastTrack() {
  if (!m_model) return false;
  return m_model->tryAliasFastTrack();
}

bool RootViewHost::applyTrailingFilter() {
  if (!m_model) return false;
  return m_model->applyTrailingFilter();
}
