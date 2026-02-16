#include "qml-manage-shortcuts-view-host.hpp"
#include "qml-manage-shortcuts-model.hpp"
#include "qml-utils.hpp"
#include "service-registry.hpp"
#include "services/shortcut/shortcut-service.hpp"
#include "services/app-service/app-service.hpp"
#include <QApplication>
#include <QClipboard>
#include <QDateTime>
#include <QUuid>

QUrl QmlManageShortcutsViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/DetailListView.qml"));
}

QVariantMap QmlManageShortcutsViewHost::qmlProperties() const {
  return {{QStringLiteral("host"),
           QVariant::fromValue(const_cast<QmlManageShortcutsViewHost *>(this))}};
}

void QmlManageShortcutsViewHost::initialize() {
  BaseView::initialize();

  m_shortcutService = ServiceRegistry::instance()->shortcuts();
  m_model = new QmlManageShortcutsModel(this);
  m_model->initialize(context());

  setSearchPlaceholderText("Search shortcuts...");

  connect(m_shortcutService, &ShortcutService::shortcutSaved, this,
          &QmlManageShortcutsViewHost::reload);
  connect(m_shortcutService, &ShortcutService::shortcutUpdated, this,
          &QmlManageShortcutsViewHost::reload);
  connect(m_shortcutService, &ShortcutService::shortcutRemoved, this,
          &QmlManageShortcutsViewHost::reload);

  connect(m_model, &QmlManageShortcutsModel::shortcutSelected, this,
          &QmlManageShortcutsViewHost::loadDetail);

  connect(context()->navigation.get(), &NavigationController::completionValuesChanged, this,
          [this](const ArgumentValues &) { updateExpandedUrl(); });

  connect(m_model, &QAbstractItemModel::modelReset, this, [this]() {
    if (m_model->rowCount() == 0) clearDetail();
  });
}

void QmlManageShortcutsViewHost::loadInitialData() { reload(); }

void QmlManageShortcutsViewHost::textChanged(const QString &text) {
  m_model->setFilter(text);
}

void QmlManageShortcutsViewHost::onReactivated() {
  m_model->refreshActionPanel();
}

void QmlManageShortcutsViewHost::beforePop() {
  clearDetail();
  m_model->beforePop();
}

QObject *QmlManageShortcutsViewHost::listModel() const { return m_model; }

void QmlManageShortcutsViewHost::loadDetail(const std::shared_ptr<Shortcut> &shortcut) {
  m_currentShortcut = shortcut;

  // Build metadata
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

  // Create completer from shortcut arguments
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

void QmlManageShortcutsViewHost::updateExpandedUrl() {
  if (!m_currentShortcut) return;

  // Gather argument values from completer
  std::vector<QString> values;
  for (const auto &val : context()->navigation->completionValues()) {
    values.emplace_back(val.second);
  }

  // Expand the URL with placeholder substitution
  QString expanded;
  size_t argumentIndex = 0;
  for (const auto &part : m_currentShortcut->parts()) {
    if (auto s = std::get_if<QString>(&part)) {
      expanded += *s;
    } else if (auto placeholder = std::get_if<Shortcut::ParsedPlaceholder>(&part)) {
      if (placeholder->id == "clipboard") {
        expanded += QApplication::clipboard()->text();
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

void QmlManageShortcutsViewHost::clearDetail() {
  if (!m_hasDetail && !m_currentShortcut) return;
  m_currentShortcut.reset();
  m_hasDetail = false;
  m_detailContent.clear();
  m_detailMetadata.clear();
  context()->navigation->destroyCurrentCompletion();
  emit detailChanged();
}

void QmlManageShortcutsViewHost::reload() {
  m_model->setItems(m_shortcutService->shortcuts());
}
