#include "oauth-overlay-host.hpp"
#include "view-utils.hpp"
#include "ui/image/proto-url.hpp"
#include "navigation-controller.hpp"
#include "overlay-controller/overlay-controller.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include "environment.hpp"
#include <QTimer>

OAuthOverlayHost::OAuthOverlayHost(const ApplicationContext *ctx,
                                   const proto::ext::oauth::AuthorizeRequest &req, QObject *parent)
    : QObject(parent), m_ctx(ctx), m_authorizeUrl(req.url().c_str()) {

  const auto &client = req.client();

  m_providerName = client.name().c_str();
  m_providerDescription = client.description().c_str();

  if (client.has_icon()) { m_providerIconSource = qml::imageSourceFor(ProtoUrl::parse(client.icon())); }
}

QUrl OAuthOverlayHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/OAuthOverlayView.qml"));
}

QString OAuthOverlayHost::providerName() const { return m_providerName; }
QString OAuthOverlayHost::providerDescription() const { return m_providerDescription; }
QString OAuthOverlayHost::providerIconSource() const { return m_providerIconSource; }
bool OAuthOverlayHost::success() const { return m_success; }

void OAuthOverlayHost::openBrowser() {
  if (Environment::isLayerShellSupported()) {
    m_ctx->navigation->closeWindow({.popToRootType = PopToRootType::Suspended});
  }

  auto appDb = m_ctx->services->appDb();
  auto wm = m_ctx->services->windowManager();

  if (auto browser = appDb->webBrowser()) {
    appDb->openTarget(m_authorizeUrl);
    wm->focusApp(*browser);
  }
}

void OAuthOverlayHost::abort() {
  m_ctx->navigation->popToRoot();
  m_ctx->overlay->dismissCurrent();
}

void OAuthOverlayHost::showSuccess() {
  using namespace std::chrono_literals;

  m_success = true;
  emit stateChanged();

  m_ctx->navigation->showWindow();

  QTimer::singleShot(2s, this, [this]() { m_ctx->overlay->dismissCurrent(); });
}
