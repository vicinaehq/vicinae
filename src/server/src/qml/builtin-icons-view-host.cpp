#include "builtin-icons-view-host.hpp"
#include "builtin-icons-model.hpp"

QUrl BuiltinIconsViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap BuiltinIconsViewHost::qmlProperties() const {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void BuiltinIconsViewHost::initialize() {
  BaseView::initialize();

  m_model = new BuiltinIconsModel(this);
  m_model->setScope(ViewScope(context(), this));
  m_model->initialize();

  setSearchPlaceholderText("Search icons...");
}

void BuiltinIconsViewHost::loadInitialData() {
  auto &mapping = BuiltinIconService::mapping();
  std::vector<IconEntry> icons;
  icons.reserve(mapping.size());
  for (const auto &[icon, name] : mapping)
    icons.push_back({icon, name});
  m_model->setItems(std::move(icons));
}

void BuiltinIconsViewHost::textChanged(const QString &text) { m_model->setFilter(text); }

void BuiltinIconsViewHost::onReactivated() { m_model->refreshActionPanel(); }

void BuiltinIconsViewHost::beforePop() { m_model->beforePop(); }

QObject *BuiltinIconsViewHost::listModel() const { return m_model; }
