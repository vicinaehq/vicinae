#include "ui/views/base-view.hpp"
#include "common.hpp"
#include "navigation-controller.hpp"
#include <qlogging.h>
#include <stdexcept>

void BaseView::createInitialize() {
  if (m_initialized) return;

  initialize();
  m_initialized = true;
}

bool BaseView::isInitialized() { return m_initialized; }

void BaseView::popSelf() {
  if (!m_ctx) return;

  m_ctx->navigation->popCurrentView();
}

void BaseView::setProxy(BaseView *proxy) {
  m_navProxy = proxy;
  setContext(proxy->context());
}

void BaseView::clearActions() { setActions(std::make_unique<ActionPanelState>()); }

void BaseView::setActions(std::unique_ptr<ActionPanelState> actions) {
  if (!m_ctx) return;
  m_ctx->navigation->setActions(std::move(actions), m_navProxy);
}

bool BaseView::supportsSearch() const { return true; }

bool BaseView::searchInteractive() const { return true; }

void BaseView::executePrimaryAction() { m_ctx->navigation->executePrimaryAction(); }

bool BaseView::needsGlobalStatusBar() const { return true; }
bool BaseView::needsGlobalTopBar() const { return true; }

void BaseView::initialize() {}

void BaseView::activate() { onActivate(); }
void BaseView::deactivate() { onDeactivate(); }

void BaseView::textChanged(const QString &text) {}

QString BaseView::navigationTitle() const {
  if (m_ctx) { return m_ctx->navigation->navigationTitle(m_navProxy); }
  return QString();
}

void BaseView::onActivate() {}
void BaseView::onDeactivate() {}

void BaseView::setContext(ApplicationContext *ctx) { m_ctx = ctx; }

ApplicationContext *BaseView::context() const {
  if (!m_ctx) {
    qCritical()
        << "BaseView::context() was called before the view was registered by the navigation controller. "
           "This is not supported and will most "
           "likely cause a crash. If you need to access the application context when a view is first "
           "shown, you should implement the `initialize` method and do what you need in there.";
  }

  return m_ctx;
}

void BaseView::destroyCompleter() {}

QString BaseView::searchPlaceholderText() const { return ""; }

void BaseView::setSearchPlaceholderText(const QString &value) const {
  if (!m_ctx) return;
  m_ctx->navigation->setSearchPlaceholderText(value, m_navProxy);
}

void BaseView::setTopBarVisiblity(bool visible) {
  if (!m_ctx) return;
  m_ctx->navigation->setHeaderVisiblity(visible, m_navProxy);
}

void BaseView::setSearchVisibility(bool visible) {
  if (!m_ctx) return;
  m_ctx->navigation->setSearchVisibility(visible, m_navProxy);
}

void BaseView::setSearchInteractive(bool interactive) {
  if (!m_ctx) return;
  m_ctx->navigation->setSearchInteractive(interactive, m_navProxy);
}

void BaseView::setStatusBarVisiblity(bool visible) {
  if (!m_ctx) return;
  m_ctx->navigation->setStatusBarVisibility(visible, m_navProxy);
}

void BaseView::clearSearchText() { setSearchText(""); }

QString BaseView::searchText() const {
  if (!m_ctx) return QString();
  return m_ctx->navigation->searchText(m_navProxy);
}

bool BaseView::isLoading() const {
  if (!m_ctx) return false;
  return m_ctx->navigation->isLoading(m_navProxy);
}

void BaseView::setSearchText(const QString &value) {
  if (!m_ctx) return;
  m_ctx->navigation->setSearchText(value, m_navProxy);
}

bool BaseView::inputFilter(QKeyEvent *event) { return false; }

void BaseView::setNavigationIcon(const ImageURL &icon) {
  if (!m_ctx) return;
  m_ctx->navigation->setNavigationIcon(icon);
}

void BaseView::setNavigation(const QString &title, const ImageURL &icon) { setNavigationTitle(title); }

void BaseView::setNavigationTitle(const QString &title) {
  if (!m_ctx) return;
  m_ctx->navigation->setNavigationTitle(title, m_navProxy);
}

void BaseView::setLoading(bool value) {
  if (!m_ctx) return;
  m_ctx->navigation->setLoading(value, m_navProxy);
}

std::vector<QString> BaseView::argumentValues() const { return {}; }

BaseView::BaseView(QObject *parent) : QObject(parent) {}
