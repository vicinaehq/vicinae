#include "builtins/screenshots/screenshots-view-host.hpp"
#include "services/screenshots/screenshot-service.hpp"
#include "services/toast/toast-service.hpp"

QVariantMap ScreenshotsViewHost::qmlProperties() {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<SectionGridModel *>(&m_model))}};
}

void ScreenshotsViewHost::initialize() {
  BaseView::initialize();
  m_model.setScope(ViewScope(context(), this));
  m_model.initialize();
  setSearchPlaceholderText(tr("Search screenshots and recordings..."));
  auto service = context()->services->screenshots();
  connect(service, &ScreenshotService::loadingChanged, this,
          [this, service] { setLoading(service->isLoading()); });
  connect(service, &ScreenshotService::refreshed, this, [this, service] {
    m_model.setItems(service->items());
    if (service->error()) context()->services->toastService()->failure(*service->error());
  });
}

void ScreenshotsViewHost::loadInitialData() {
  m_model.setFilter(searchText());
  context()->services->screenshots()->refresh();
}

void ScreenshotsViewHost::onReactivated() { context()->services->screenshots()->refresh(); }

void ScreenshotsViewHost::textChanged(const QString &text) { m_model.setFilter(text); }
