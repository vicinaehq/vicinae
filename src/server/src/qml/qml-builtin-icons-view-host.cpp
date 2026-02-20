#include "qml-builtin-icons-view-host.hpp"
#include "qml-builtin-icons-model.hpp"

QUrl QmlBuiltinIconsViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap QmlBuiltinIconsViewHost::qmlProperties() const {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void QmlBuiltinIconsViewHost::initialize() {
  BaseView::initialize();

  m_model = new QmlBuiltinIconsModel(this);
  m_model->initialize(context());

  setSearchPlaceholderText("Search icons...");
}

void QmlBuiltinIconsViewHost::loadInitialData() {
  auto &mapping = BuiltinIconService::mapping();
  std::vector<IconEntry> icons;
  icons.reserve(mapping.size());
  for (const auto &[icon, name] : mapping)
    icons.push_back({icon, name});
  m_model->setItems(std::move(icons));
}

void QmlBuiltinIconsViewHost::textChanged(const QString &text) { m_model->setFilter(text); }

void QmlBuiltinIconsViewHost::onReactivated() { m_model->refreshActionPanel(); }

void QmlBuiltinIconsViewHost::beforePop() { m_model->beforePop(); }

QObject *QmlBuiltinIconsViewHost::listModel() const { return m_model; }
