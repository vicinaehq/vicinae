#include "manage-shortcuts-view-host.hpp"
#include "manage-shortcuts-model.hpp"
#include "view-utils.hpp"
#include "service-registry.hpp"
#include "services/shortcut/shortcut-service.hpp"
#include "services/app-service/app-service.hpp"
#include <QGuiApplication>
#include <QClipboard>
#include <QDateTime>
#include <QUuid>

QUrl ManageShortcutsViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/DetailListView.qml"));
}

QVariantMap ManageShortcutsViewHost::qmlProperties() const {
  return {{QStringLiteral("host"),
           QVariant::fromValue(const_cast<ManageShortcutsViewHost *>(this))}};
}

void ManageShortcutsViewHost::initialize() {
  BaseView::initialize();

  m_shortcutService = ServiceRegistry::instance()->shortcuts();
  m_model = new ManageShortcutsModel(this);
  m_model->setScope(ViewScope(context(), this));
  m_model->initialize();

  setSearchPlaceholderText("Search shortcuts...");

  connect(m_shortcutService, &ShortcutService::shortcutSaved, this,
          &ManageShortcutsViewHost::reload);
  connect(m_shortcutService, &ShortcutService::shortcutUpdated, this,
          &ManageShortcutsViewHost::reload);
  connect(m_shortcutService, &ShortcutService::shortcutRemoved, this,
          &ManageShortcutsViewHost::reload);

  connect(m_model, &ManageShortcutsModel::shortcutSelected, this,
          &ManageShortcutsViewHost::loadDetail);

  connect(context()->navigation.get(), &NavigationController::completionValuesChanged, this,
          [this](const ArgumentValues &) { updateExpandedUrl(); });

  connect(m_model, &QAbstractItemModel::modelReset, this, [this]() {
    if (m_model->rowCount() == 0) clearDetail();
  });
}

void ManageShortcutsViewHost::loadInitialData() { reload(); }

void ManageShortcutsViewHost::textChanged(const QString &text) {
  m_model->setFilter(text);
}

void ManageShortcutsViewHost::onReactivated() {
  m_model->refreshActionPanel();
}

void ManageShortcutsViewHost::beforePop() {
  clearDetail();
  m_model->beforePop();
}

QObject *ManageShortcutsViewHost::listModel() const { return m_model; }

void ManageShortcutsViewHost::loadDetail(const std::shared_ptr<Shortcut> &shortcut) {
  m_currentShortcut = shortcut;

  auto appDb = ServiceRegistry::instance()->appDb();
  QVariantList meta;

  meta.append(QVariantMap{
      {QStringLiteral("label"), QStringLiteral("Name")},
      {QStringLiteral("value"), shortcut->name()},
  });

  if (auto app = appDb->findById(shortcut->app())) {
    meta.append(QVariantMap{
        {QStringLiteral("label"), QStringLiteral("Application")},
        {QStringLiteral("value"), app->displayName()},
        {QStringLiteral("icon"), qml::imageSourceFor(app->iconUrl())},
    });
  }

  meta.append(QVariantMap{
      {QStringLiteral("label"), QStringLiteral("Opened")},
      {QStringLiteral("value"), QString::number(shortcut->openCount())},
  });

  meta.append(QVariantMap{
      {QStringLiteral("label"), QStringLiteral("Last Opened")},
      {QStringLiteral("value"),
       shortcut->lastOpenedAt() ? shortcut->lastOpenedAt()->toString() : QStringLiteral("Never")},
  });

  meta.append(QVariantMap{
      {QStringLiteral("label"), QStringLiteral("Created at")},
      {QStringLiteral("value"), shortcut->createdAt().toString()},
  });

  m_detailMetadata = meta;

  ArgumentList args;
  for (const auto &arg : shortcut->arguments()) {
    args.emplace_back(CommandArgument{
        .name = arg.name,
        .type = CommandArgument::Text,
        .placeholder = arg.name,
        .required = arg.defaultValue.isEmpty(),
    });
  }

  if (!args.empty()) {
    context()->navigation->createCompletion(args, shortcut->icon());
  } else {
    context()->navigation->destroyCurrentCompletion();
  }

  updateExpandedUrl();
}

void ManageShortcutsViewHost::updateExpandedUrl() {
  if (!m_currentShortcut) return;

  std::vector<QString> values;
  for (const auto &val : context()->navigation->completionValues()) {
    values.emplace_back(val.second);
  }

  QString expanded;
  size_t argumentIndex = 0;
  for (const auto &part : m_currentShortcut->parts()) {
    if (auto s = std::get_if<QString>(&part)) {
      expanded += *s;
    } else if (auto placeholder = std::get_if<Shortcut::ParsedPlaceholder>(&part)) {
      if (placeholder->id == "clipboard") {
        expanded += QGuiApplication::clipboard()->text();
      } else if (placeholder->id == "selected") {
        // TODO: selected text
      } else if (placeholder->id == "uuid") {
        expanded += QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
      } else {
        if (argumentIndex < values.size()) {
          expanded += values.at(argumentIndex++);
        }
      }
    }
  }

  m_detailContent = expanded;
  m_hasDetail = true;
  emit detailChanged();
}

void ManageShortcutsViewHost::clearDetail() {
  if (!m_hasDetail && !m_currentShortcut) return;
  m_currentShortcut.reset();
  m_hasDetail = false;
  m_detailContent.clear();
  m_detailMetadata.clear();
  context()->navigation->destroyCurrentCompletion();
  emit detailChanged();
}

void ManageShortcutsViewHost::reload() {
  m_model->setItems(m_shortcutService->shortcuts());
}
