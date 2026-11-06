#include "quick-ai-view-host.hpp"
#include <utility>
#include "builtins/ai/ai.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/ai/conversation-store.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/windows/chat-window-controller.hpp"
#include "view-utils.hpp"
#include "vicinae.hpp"

QuickAIViewHost::QuickAIViewHost(ServiceRegistry &services, QString initialQuery)
    : m_initialQuery(std::move(initialQuery)), m_session(new ChatSession(services, this)) {}

QuickAIViewHost::~QuickAIViewHost() { beforePop(); }

void QuickAIViewHost::beforePop() {
  if (m_session && m_session->parent() == this) m_session->cancel();
}

void QuickAIViewHost::openConversation(BaseView *source, std::string id) {
  if (source->isLoading()) return;
  auto *chat = source->context()->chat.get();
  if (const auto *session = chat->session(); session && session->conversationId().toStdString() == id) {
    chat->show();
    source->context()->navigation->closeWindow();
    return;
  }

  auto *view = new QuickAIViewHost(*source->context()->services);
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
  setNavigationTitle(initialNavigationTitle());
  connect(m_session, &ChatSession::conversationChanged, this,
          [this] { setNavigationTitle(initialNavigationTitle()); });
}

void QuickAIViewHost::loadInitialData() {
  if (m_opening) {
    openLastConversation();
    return;
  }
  if (!m_initialQuery.isEmpty() && !m_session->send(m_initialQuery)) m_session->setDraft(m_initialQuery);
}

QuickAIViewHost *QuickAIViewHost::forLastConversation(ServiceRegistry &services) {
  auto *view = new QuickAIViewHost(services);
  view->m_opening = true;
  return view;
}

void QuickAIViewHost::openLastConversation() {
  auto *ctx = context();
  ctx->services->conversations()->list().then(
      this, [this, ctx](AI::Result<std::vector<AI::ConversationInfo>> result) {
        setOpening(false);
        const auto *top = ctx->navigation->topState();
        if (!top || top->sender != this) return;

        if (!result) {
          leaveWithError(tr("Could not load conversations."));
          return;
        }

        if (result->empty()) {
          ctx->navigation->activateEntrypoint(
              EntrypointId{std::string(QuickAI::REPOSITORY_ID), std::string(QuickAI::COMMAND_ID)});
          return;
        }

        auto id = std::move(result->front().id);
        auto *chat = ctx->chat.get();

        // The chat window may be streaming this conversation, a second copy would diverge from it.
        if (const auto *session = chat->session(); session && session->conversationId().toStdString() == id) {
          chat->show();
          ctx->navigation->popCurrentView();
          ctx->navigation->closeWindow();
          return;
        }

        connect(m_session, &ChatSession::loadFailed, this, &QuickAIViewHost::leaveWithError);
        m_session->load(std::move(id));
      });
}

void QuickAIViewHost::setOpening(bool value) {
  if (std::exchange(m_opening, value) != value) emit openingChanged();
}

void QuickAIViewHost::leaveWithError(const QString &message) {
  auto *ctx = context();
  ctx->services->toastService()->failure(message);
  const auto *top = ctx->navigation->topState();
  if (top && top->sender == this) ctx->navigation->popCurrentView();
}

void QuickAIViewHost::openInWindow() {
  context()->chat->show(m_session);
  context()->navigation->popCurrentView();
  context()->navigation->closeWindow();
}

QString QuickAIViewHost::initialNavigationTitle() const {
  return m_session && !m_session->conversationId().isEmpty() ? m_session->title() : tr("Quick AI");
}

ImageURL QuickAIViewHost::initialNavigationIcon() const { return QuickAI::ICON; }

QUrl QuickAIViewHost::qmlComponentUrl() const { return qml::componentUrl(u"QuickAIView"); }

QVariantMap QuickAIViewHost::qmlProperties() { return {{QStringLiteral("host"), QVariant::fromValue(this)}}; }
