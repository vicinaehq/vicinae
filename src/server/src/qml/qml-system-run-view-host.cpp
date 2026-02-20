#include "qml-system-run-view-host.hpp"
#include "qml-system-run-model.hpp"
#include "xdgpp/desktop-entry/exec.hpp"

QUrl QmlSystemRunViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap QmlSystemRunViewHost::qmlProperties() const {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void QmlSystemRunViewHost::initialize() {
  BaseView::initialize();

  m_model = new QmlSystemRunModel(this);
  m_model->initialize(context());
  m_model->setDefaultAction(
      QmlSystemRunModel::parseDefaultAction(command()->preferenceValues().value("default-action").toString()));

  setSearchPlaceholderText("Search for a program to execute...");
  setLoading(true);

  connect(&m_programDb, &ProgramDb::backgroundScanFinished, this, [this]() {
    setLoading(false);
    refresh(searchText());
  });
  m_programDb.backgroundScan();
}

void QmlSystemRunViewHost::loadInitialData() {}

void QmlSystemRunViewHost::textChanged(const QString &text) { refresh(text); }

void QmlSystemRunViewHost::onReactivated() { m_model->refreshActionPanel(); }

QObject *QmlSystemRunViewHost::listModel() const { return m_model; }

void QmlSystemRunViewHost::refresh(const QString &text) {
  auto str = text.trimmed().toStdString();
  auto parsed = xdgpp::ExecParser("").parse(str);
  bool hasProg = false;

  if (!parsed.empty()) hasProg = ProgramDb::programPath(parsed.front()).has_value();

  m_model->setData(std::move(parsed), hasProg, m_programDb.search(str, 100));
}
