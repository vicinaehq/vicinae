#include "manage-models-view-host.hpp"
#include "manage-models-model.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-service.hpp"

QUrl ManageModelsViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/DetailListView.qml"));
}

QVariantMap ManageModelsViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void ManageModelsViewHost::initialize() {
  BaseView::initialize();

  m_aiService = ServiceRegistry::instance()->ai();
  m_model = new ManageModelsModel(this);
  m_model->setScope(ViewScope(context(), this));
  m_model->initialize();

  setSearchPlaceholderText("Search models...");

  connect(m_aiService, &AI::Service::modelsChanged, this, &ManageModelsViewHost::reload);
  connect(m_model, &ManageModelsModel::modelSelected, this, &ManageModelsViewHost::loadDetail);

  connect(m_model, &QAbstractItemModel::modelReset, this, [this]() {
    if (m_model->rowCount() == 0) clearDetail();
  });
}

void ManageModelsViewHost::loadInitialData() { reload(); }

void ManageModelsViewHost::textChanged(const QString &text) { m_model->setFilter(text); }

QObject *ManageModelsViewHost::listModel() const { return m_model; }

void ManageModelsViewHost::loadDetail(const AI::ProviderModel &model) {
  QVariantList meta;

  meta.append(QVariantMap{
      {QStringLiteral("label"), QStringLiteral("Model ID")},
      {QStringLiteral("value"), QString::fromStdString(model.id)},
  });

  meta.append(QVariantMap{
      {QStringLiteral("label"), QStringLiteral("Provider")},
      {QStringLiteral("value"), QString::fromStdString(model.ref.provider)},
  });

  auto caps = AI::stringifyCapabilities(model.caps);
  if (!caps.empty()) {
    QVariantList tags;
    for (const auto &cap : caps) {
      tags.append(QVariantMap{{QStringLiteral("text"), QString::fromStdString(cap)}});
    }

    meta.append(QVariantMap{
        {QStringLiteral("type"), QStringLiteral("tags")},
        {QStringLiteral("label"), QStringLiteral("Capabilities")},
        {QStringLiteral("tags"), tags},
    });
  }

  m_detailMetadata = meta;
  m_hasDescription = model.description.has_value();
  m_detailContent = m_hasDescription ? QString::fromStdString(*model.description) : QString();
  m_hasDetail = true;
  emit detailChanged();
}

void ManageModelsViewHost::clearDetail() {
  if (!m_hasDetail) return;
  m_hasDetail = false;
  m_hasDescription = false;
  m_detailContent.clear();
  m_detailMetadata.clear();
  emit detailChanged();
}

void ManageModelsViewHost::reload() { m_model->setItems(m_aiService->listModels()); }
