#include "root-view-host.hpp"
#include "root-search-model.hpp"
#include "section-source.hpp"
#include "view-scope.hpp"
#include <qevent.h>

void RootViewHost::initialize() {
  using namespace std::chrono_literals;

  BaseView::initialize();
  m_model = new RootSearchModel(ViewScope(context(), this), this);
  m_clockTimer->setInterval(1min);
  m_clockTimer->start();
  refreshClock();

  connect(m_clockTimer, &QTimer::timeout, this, &RootViewHost::refreshClock);
  connect(m_model, &SectionListModel::itemSelected, this, [this](SectionSource *source, int itemIdx) {
    if (auto panel = source->actionPanel(itemIdx))
      setActions(std::move(panel));
    else
      clearActions();
  });
  connect(m_model, &SectionListModel::selectionCleared, this, [this]() { clearActions(); });

  m_model->setFilter({});
}

void RootViewHost::refreshClock() {
  QLocale locale;
  QString timeStr = locale.toString(QTime::currentTime(), QLocale::ShortFormat);
  ViewScope scope(context(), this);
  scope.setNavigationTitle(timeStr);
}

QUrl RootViewHost::qmlComponentUrl() const { return QUrl(QStringLiteral("qrc:/Vicinae/RootSearchList.qml")); }

void RootViewHost::beforeActionExecuted(const AbstractAction *action) {
  auto manager = context()->services->rootItemManager();
  auto text = context()->navigation->searchText(this);

  manager->searchHistory().add(text.toStdString());

  if (auto item = m_model->selectedRootItem()) { manager->registerVisit(item->uniqueId()); }
}

bool RootViewHost::inputFilter(QKeyEvent *event) {
  auto manager = context()->services->rootItemManager();
  auto &nav = context()->navigation;
  // wrapped navigation is incompatible with overriding key up, so we disable history in that case
  const bool activatableHistory = !context()->services->config()->value().wrapNavigation &&
                                  m_model->selectedIndex() == m_model->nextSelectableIndex(-1, 1);
  const bool shouldCycleHistory = !event->modifiers() && event->key() == Qt::Key_Up && activatableHistory;

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

bool RootViewHost::tryAliasFastTrack() {
  if (!m_model) return false;
  return m_model->tryAliasFastTrack();
}
