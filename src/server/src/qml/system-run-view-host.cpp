#include "system-run-view-host.hpp"
#include "system-run-model.hpp"
#include "xdgpp/desktop-entry/exec.hpp"

QUrl SystemRunViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap SystemRunViewHost::qmlProperties() const {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void SystemRunViewHost::initialize() {
  BaseView::initialize();

  m_model = new SystemRunModel(this);
  m_model->initialize(context());
  m_model->setDefaultAction(
      SystemRunModel::parseDefaultAction(command()->preferenceValues().value("default-action").toString()));

  setSearchPlaceholderText("Search for a program to execute...");
  setLoading(true);

  connect(&m_programDb, &ProgramDb::backgroundScanFinished, this, [this]() {
    setLoading(false);
    refresh(searchText());
  });
  m_programDb.backgroundScan();
}

void SystemRunViewHost::loadInitialData() {}

void SystemRunViewHost::textChanged(const QString &text) { refresh(text); }

void SystemRunViewHost::onReactivated() { m_model->refreshActionPanel(); }

QObject *SystemRunViewHost::listModel() const { return m_model; }

void SystemRunViewHost::refresh(const QString &text) {
  auto str = text.trimmed().toStdString();
  auto parsed = xdgpp::ExecParser("").parse(str);
  bool hasProg = false;

  if (!parsed.empty()) hasProg = ProgramDb::programPath(parsed.front()).has_value();

  m_model->setData(std::move(parsed), hasProg, m_programDb.search(str, 100));
}
