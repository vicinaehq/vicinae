#include "quick-ai-view-host.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/windows/chat-window.hpp"
#include "view-utils.hpp"
#include "vicinae.hpp"

QuickAIViewHost::QuickAIViewHost(QString initialQuery, std::optional<AI::ModelRef> model)
    : m_initialQuery(std::move(initialQuery)), m_session(new ChatSession(std::move(model), this)) {}

QuickAIViewHost::~QuickAIViewHost() { beforePop(); }

void QuickAIViewHost::beforePop() {
  if (m_session && m_session->parent() == this) m_session->cancel();
}

void QuickAIViewHost::openConversation(BaseView *source, std::string id) {
  if (source->isLoading()) return;
  auto *window = source->context()->chat.get();
  if (auto *session = window->findSession(id)) {
    window->show(session);
    source->context()->navigation->closeWindow();
    return;
  }

  auto *view = new QuickAIViewHost({}, std::nullopt);
  view->setParent(source);
  source->setLoading(true);
  connect(view->session(), &ChatSession::conversationLoaded, source, [source, view] {
    source->setLoading(false);
    auto *navigation = source->context()->navigation.get();
    const auto *state = navigation->topState();
    if (!state || state->sender != source) {
      view->deleteLater();
      return;
    }
    view->setParent(nullptr);
    navigation->pushView(view);
  });
  connect(view->session(), &ChatSession::loadFailed, source, [source, view](const QString &message) {
    source->setLoading(false);
    source->context()->services->toastService()->failure(message);
    view->deleteLater();
  });
  view->session()->load(std::move(id));
}

void QuickAIViewHost::initialize() {
  BaseView::initialize();
  context()->chat->trackSession(m_session);
  setNavigationTitle(initialNavigationTitle());
  connect(m_session, &ChatSession::conversationChanged, this,
          [this] { setNavigationTitle(initialNavigationTitle()); });
}

void QuickAIViewHost::loadInitialData() {
  if (!m_initialQuery.isEmpty()) m_session->send(m_initialQuery);
}

void QuickAIViewHost::openInWindow() {
  context()->chat->show(m_session);
  context()->navigation->popCurrentView();
  context()->navigation->closeWindow();
}

QString QuickAIViewHost::initialNavigationTitle() const {
  return m_session && !m_session->conversationId().isEmpty() ? m_session->title() : tr("Quick AI");
}

ImageURL QuickAIViewHost::initialNavigationIcon() const {
  return ImageURL::builtin(BuiltinIcon::Stars).setBackgroundTint(Omnicast::ACCENT_COLOR);
}

QUrl QuickAIViewHost::qmlComponentUrl() const { return qml::componentUrl(u"QuickAIView"); }

QVariantMap QuickAIViewHost::qmlProperties() { return {{QStringLiteral("host"), QVariant::fromValue(this)}}; }
