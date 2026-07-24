#include "root-view-host.hpp"
#include "root-search-model.hpp"
#include "section-source.hpp"
#include "service-registry.hpp"
#include "services/keybinding/keybinding-service.hpp"
#include "view-scope.hpp"
#include <qevent.h>

void RootViewHost::initialize() {
  using namespace std::chrono_literals;
  auto cfgService = context()->services->config();

  BaseView::initialize();
  m_model = new RootSearchModel(ViewScope(context(), this), this);
  scheduleNextClockTick();

  connect(cfgService, &config::Manager::configChanged, this,
          [this](const auto &next, const auto &prev) { scheduleNextClockTick(); });

  connect(m_clockTimer, &QTimer::timeout, this, &RootViewHost::scheduleNextClockTick);
  connect(m_model, &SectionListModel::itemSelected, this, [this](SectionSource *source, int itemIdx) {
    if (auto panel = source->actionPanel(itemIdx))
      setActions(std::move(panel));
    else
      clearActions();
  });
  connect(m_model, &SectionListModel::selectionCleared, this, [this]() { clearActions(); });

  m_model->setFilter({});
}

void RootViewHost::scheduleNextClockTick() {
  auto &config = context()->services->config()->value();
  auto &cc = config.launcherWindow.clock;
  ViewScope scope(context(), this);

  if (!cc.enabled) {
    context()->navigation->setNavigationTitle("");
    m_clockTimer->stop();
    return;
  }

  {
    QLocale locale;
    QString timeStr;

    if (cc.format) {
      timeStr = locale.toString(QDateTime::currentDateTime(), QString::fromStdString(*cc.format));
    } else {
      timeStr = locale.toString(QTime::currentTime(), QLocale::ShortFormat);
    }

    scope.setNavigationTitle(timeStr);
  }

  auto delta = cc.interval - (QDateTime::currentSecsSinceEpoch() % cc.interval);

  m_clockTimer->setInterval(std::chrono::seconds(delta));
  m_clockTimer->setSingleShot(true);
  m_clockTimer->start();
}

QUrl RootViewHost::qmlComponentUrl() const { return QUrl(QStringLiteral("qrc:/Vicinae/RootSearchList.qml")); }

void RootViewHost::beforeActionExecuted(const AbstractAction *action) {
  auto manager = context()->services->rootItemManager();
  auto text = context()->navigation->searchText(this);

  manager->searchHistory().add(text.toStdString());

  if (auto item = m_model->selectedRootItem()) { manager->registerVisit(item->uniqueId()); }
}

bool RootViewHost::tryAliasFastTrack() {
  const auto manager = context()->services->rootItemManager();
  const auto item = m_model->selectedRootItem();

  if (!item || !item->supportsAliasSpaceShortcut()) return false;

  const auto query = context()->navigation->searchText(this).toStdString();
  const auto meta = manager->itemMetadata(item->uniqueId());

  if (!meta.alias || !meta.alias->starts_with(query)) return false;

  m_model->activateSelected();
  return true;
}

bool RootViewHost::inputFilter(QKeyEvent *event) {
  auto manager = context()->services->rootItemManager();
  auto &nav = context()->navigation;
  auto &cfg = context()->services->config()->value();

  if (!event->modifiers() && event->key() == Qt::Key_Space) { return tryAliasFastTrack(); }

  // wrapped navigation is incompatible with overriding key up, so we disable history in that case
  const bool activatableHistory =
      !cfg.wrapNavigation && m_model->selectedIndex() == m_model->nextSelectableIndex(-1, 1);
  const bool shouldCycleHistory =
      (event->key() == Qt::Key_Up || KeyBindingService::isUpKey(event, cfg.keybinding)) && activatableHistory;

  if (shouldCycleHistory) {
    if (!m_historyOffset) {
      m_historyOffset = 0;
    } else {
      *m_historyOffset += 1;
    }

    auto entry = manager->searchHistory().at(*m_historyOffset);

    // skip entry if it's already our search text, we don't care
    while (entry && QString::fromStdString(entry->q) == nav->searchText()) {
      *m_historyOffset += 1;
      entry = manager->searchHistory().at(*m_historyOffset);
    }

    if (entry) {
      m_textChangedByHistory = true;
      nav->setSearchText(QString::fromStdString(entry->q));
      m_textChangedByHistory = false;
    }

    return true;
  }

  return false;
}

QVariantMap RootViewHost::qmlProperties() {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void RootViewHost::textChanged(const QString &text) {
  if (!m_textChangedByHistory) { m_historyOffset.reset(); }
  if (m_model) m_model->setFilter(text);
}

void RootViewHost::onReactivated() {
  if (m_model) m_model->refreshActionPanel();
}

void RootViewHost::beforePop() {
  if (m_model) m_model->beforePop();
}

QObject *RootViewHost::listModel() const { return m_model; }
