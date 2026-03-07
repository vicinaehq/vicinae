#include "quick-ai-view-host.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-service.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/paste/paste-service.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/image/url.hpp"

QuickAIViewHost::QuickAIViewHost(QString initialQuery) : m_initialQuery(std::move(initialQuery)) {}

QUrl QuickAIViewHost::qmlComponentUrl() const { return QUrl(QStringLiteral("qrc:/Vicinae/QuickAIView.qml")); }

QVariantMap QuickAIViewHost::qmlProperties() { return {{QStringLiteral("host"), QVariant::fromValue(this)}}; }

void QuickAIViewHost::initialize() {
  BaseView::initialize();

  m_aiService = ServiceRegistry::instance()->ai();

  setSearchPlaceholderText("Ask a follow-up...");
  setNavigationTitle("Quick AI");

  m_history.emplace_back(AI::ChatMessage{
      .role = AI::ChatRole::System,
      .value = "You are a concise assistant integrated into a desktop launcher. "
               "Give direct, helpful answers. Prefer short responses unless detail is asked for.",
  });

  updateActions();
}

void QuickAIViewHost::loadInitialData() { sendQuery(m_initialQuery.toStdString()); }

void QuickAIViewHost::textChanged(const QString &text) {
  bool hadText = !m_followUpText.isEmpty();
  m_followUpText = text;
  bool hasText = !m_followUpText.isEmpty();

  if (hadText != hasText && !m_streaming) updateActions();
}

void QuickAIViewHost::sendQuery(const std::string &query) {
  m_history.emplace_back(AI::ChatMessage{.role = AI::ChatRole::User, .value = query});

  m_currentResponse.clear();
  m_streamingQuery = QString::fromStdString(query);
  m_streamingContent.clear();
  m_streaming = true;
  emit streamingChanged();
  emit streamingContentChanged();

  AI::ChatCompletionPayload payload;
  payload.messages = m_history;

  m_stream = m_aiService->createChatCompletion(payload);

  if (!m_stream) {
    m_streaming = false;
    emit streamingChanged();
    return;
  }

  const auto &model = m_stream->model();
  m_modelLabel = QString::fromStdString(model.name);
  m_modelIcon = model.icon.value_or(ImageUrl{});
  emit modelChanged();

  connect(m_stream.get(), &AI::AbstractChatCompletionStream::dataAdded, this,
          [this](const std::string &text) {
            m_currentResponse += text;
            m_streamingContent = QString::fromStdString(m_currentResponse);
            emit streamingContentChanged();
          });

  connect(m_stream.get(), &AI::AbstractChatCompletionStream::finished, this, [this]() {
    m_streaming = false;
    emit streamingChanged();

    m_history.emplace_back(AI::ChatMessage{.role = AI::ChatRole::Assistant, .value = m_currentResponse});

    m_exchanges.append(QVariantMap{
        {QStringLiteral("query"), m_streamingQuery},
        {QStringLiteral("response"), m_streamingContent},
    });
    emit exchangesChanged();

    m_streamingContent.clear();
    m_streamingQuery.clear();
    emit streamingContentChanged();
    m_stream.reset();

    updateActions();
  });

  connect(m_stream.get(), &AI::AbstractChatCompletionStream::errorOccured, this,
          [this](const std::string &reason) {
            m_streaming = false;
            emit streamingChanged();
            m_stream.reset();

            updateActions();
          });

  m_stream->start();
  updateActions();
}

void QuickAIViewHost::updateActions() {
  auto panel = std::make_unique<ActionPanelState>();
  auto *section = panel->createSection();

  if (m_streaming) {
    auto *cancel = new StaticAction(QStringLiteral("Cancel"), ImageURL::builtin("x-circle"), [this]() {
      if (m_stream) m_stream->abort();
    });
    cancel->setPrimary(true);
    section->addAction(cancel);
  } else if (!m_followUpText.isEmpty()) {
    auto *followUp =
        new StaticAction(QStringLiteral("Send Follow-up"), ImageURL::builtin("arrow-up"), [this]() {
          auto query = m_followUpText.toStdString();
          m_followUpText.clear();
          clearSearchText();
          sendQuery(query);
        });
    followUp->setPrimary(true);
    section->addAction(followUp);

    auto *paste = new StaticAction(QStringLiteral("Paste to App"), ImageURL::builtin("clipboard-text"),
                                   [this]() { pasteLastResponse(); });
    section->addAction(paste);
  } else {
    auto *paste = new StaticAction(QStringLiteral("Paste to App"), ImageURL::builtin("clipboard-text"),
                                   [this]() { pasteLastResponse(); });
    paste->setPrimary(true);
    section->addAction(paste);
  }

  setActions(std::move(panel));
}

void QuickAIViewHost::pasteLastResponse() {
  if (m_history.empty()) return;

  auto &last = m_history.back();
  if (last.role != AI::ChatRole::Assistant) return;

  auto *paste = ServiceRegistry::instance()->pasteService();
  paste->pasteContent(Clipboard::Text{QString::fromStdString(last.value)});
  context()->navigation->closeWindow();
}
