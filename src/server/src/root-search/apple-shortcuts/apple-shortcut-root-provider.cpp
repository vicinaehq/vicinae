#include <QDesktopServices>
#include <QGuiApplication>
#include <QUrlQuery>

#include "apple-shortcut-root-provider.hpp"
#include "actions/root-search-actions.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"

namespace {

ImageURL shortcutsIcon() { return ImageURL::macBundle("/System/Applications/Shortcuts.app"); }

class RunAppleShortcutAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(RunAppleShortcutAction)

public:
  explicit RunAppleShortcutAction(std::string id)
      : AbstractAction(tr("Run Shortcut"), ImageURL::builtin(BuiltinIcon::Play)), m_id(std::move(id)) {}

  void execute(ApplicationContext *ctx) override {
    auto *toast = ctx->services->toastService();
    AppleShortcuts::run(m_id).then(toast, [toast](const AppleShortcuts::RunResult &result) {
      if (!result) toast->failure(tr("Failed to start shortcut"), result.error());
    });
    ctx->navigation->closeWindow({.clearRootSearch = true});
  }

private:
  std::string m_id;
};

class EditAppleShortcutAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(EditAppleShortcutAction)

public:
  explicit EditAppleShortcutAction(std::string id)
      : AbstractAction(tr("Edit in Shortcuts"), ImageURL::builtin(BuiltinIcon::Pencil)), m_id(std::move(id)) {
    setShortcut(Keybind::EditAction);
  }

  void execute(ApplicationContext *ctx) override {
    QUrl url("shortcuts://open-shortcut");
    QUrlQuery query;
    query.addQueryItem("id", QString::fromStdString(m_id));
    url.setQuery(query);
    if (!QDesktopServices::openUrl(url)) {
      ctx->services->toastService()->failure(tr("Failed to open shortcut"));
      return;
    }
    ctx->navigation->closeWindow({.clearRootSearch = true});
  }

private:
  std::string m_id;
};

} // namespace

AppleShortcutRootItem::AppleShortcutRootItem(AppleShortcuts::Shortcut shortcut)
    : m_shortcut(std::move(shortcut)),
      m_icon(m_shortcut.icon.isEmpty() ? shortcutsIcon() : ImageURL::rawData(m_shortcut.icon, "image/png")) {}

EntrypointId AppleShortcutRootItem::uniqueId() const { return {"apple-shortcuts", m_shortcut.id}; }
QString AppleShortcutRootItem::title() const { return QString::fromStdString(m_shortcut.name); }
QString AppleShortcutRootItem::typeDisplayName() const { return tr("Apple Shortcut"); }
ImageURL AppleShortcutRootItem::iconUrl() const { return m_icon; }
AccessoryList AppleShortcutRootItem::accessories() const {
  return {{.text = tr("Apple Shortcut"), .color = SemanticColor::TextMuted}};
}

std::unique_ptr<ActionPanelState>
AppleShortcutRootItem::newActionPanel(ApplicationContext *ctx, const RootItemMetadata &metadata) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *main = panel->createSection();
  main->addAction(new RunAppleShortcutAction(m_shortcut.id));
  main->addAction(new EditAppleShortcutAction(m_shortcut.id));
  auto *items = panel->createSection();
  for (const auto &action :
       RootSearchActionGenerator::generateActions(*this, *ctx->services->rootItemManager())) {
    items->addAction(action);
  }
  panel->setTitle(title());
  return panel;
}

AppleShortcutRootProvider::AppleShortcutRootProvider(ToastService &toastService)
    : m_toastService(toastService) {
  using namespace std::chrono_literals;
  m_refreshTimer.setInterval(1min);
  connect(&m_refreshTimer, &QTimer::timeout, this, &AppleShortcutRootProvider::refresh);
  connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, [this](Qt::ApplicationState state) {
    if (state == Qt::ApplicationActive) refresh();
  });
  connect(&m_scanWatcher, &QFutureWatcherBase::finished, this, [this]() {
    auto result = m_scanWatcher.future().takeResult();
    if (!result) {
      if (m_lastError != result.error()) {
        m_toastService.failure(tr("Could not load Apple Shortcuts"), result.error());
        qWarning() << "Could not load Apple Shortcuts:" << result.error();
      }
      m_lastError = result.error();
      return;
    }
    m_lastError.reset();
    if (m_shortcuts == *result) return;
    m_shortcuts = std::move(*result);
    emit itemsChanged();
  });
}

QString AppleShortcutRootProvider::uniqueId() const { return "apple-shortcuts"; }
QString AppleShortcutRootProvider::displayName() const { return tr("Apple Shortcuts"); }
ImageURL AppleShortcutRootProvider::icon() const { return shortcutsIcon(); }
RootProvider::Type AppleShortcutRootProvider::type() const { return GroupProvider; }

void AppleShortcutRootProvider::initialized(const QJsonObject &) {
  m_refreshTimer.start();
  refresh();
}

void AppleShortcutRootProvider::refresh() {
  if (m_scanWatcher.isRunning() || (m_lastRefresh.isValid() && m_lastRefresh.elapsed() < 5000)) return;
  m_lastRefresh.start();
  m_scanWatcher.setFuture(AppleShortcuts::list());
}

std::vector<std::shared_ptr<RootItem>> AppleShortcutRootProvider::loadItems() const {
  std::vector<std::shared_ptr<RootItem>> items;
  items.reserve(m_shortcuts.size());
  for (const auto &shortcut : m_shortcuts) {
    items.emplace_back(std::make_shared<AppleShortcutRootItem>(shortcut));
  }
  return items;
}
