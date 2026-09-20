#include "quick-ai-view-host.hpp"
#include "ai-model-selector-utils.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-service.hpp"
#include "services/dictation/dictation-service.hpp"
#include "services/dictation/transcription-session.hpp"
#include "services/permissions/macos-permission-service.hpp"
#include "ui/image/url.hpp"
#include "view-utils.hpp"

namespace {
constexpr int DICTATION_MESSAGE_MS = 2500;
}

QuickAIViewHost::QuickAIViewHost(QString initialQuery, AI::ModelRef model)
    : m_initialQuery(std::move(initialQuery)), m_selectedModel(std::move(model)) {}

QUrl QuickAIViewHost::qmlComponentUrl() const { return qml::componentUrl(u"QuickAIView"); }

QVariantMap QuickAIViewHost::qmlProperties() { return {{QStringLiteral("host"), QVariant::fromValue(this)}}; }

void QuickAIViewHost::initialize() {
  BaseView::initialize();

  m_aiService = ServiceRegistry::instance()->ai();

  setNavigationTitle(tr("Quick AI"));

  m_history.emplace_back(AI::ChatMessage::fromText(
      AI::ChatRole::System,
      "You are a concise assistant integrated into a desktop launcher. "
      "Give direct, helpful answers. Prefer short responses unless detail is asked for."));

  connect(m_aiService, &AI::Service::modelsChanged, this, &QuickAIViewHost::rebuildModelSelectorItems);
  rebuildModelSelectorItems();

  m_dictationService = ServiceRegistry::instance()->dictation();
  m_dictation = new TranscriptionSession(*m_dictationService, this);
  connect(m_dictation, &TranscriptionSession::stateChanged, this, &QuickAIViewHost::dictationStateChanged);
  connect(m_dictation, &TranscriptionSession::elapsedTimeChanged, this,
          &QuickAIViewHost::recordingTimeChanged);
  connect(m_dictation, &TranscriptionSession::transcribed, this,
          [this](const Transcript &transcript) { emit dictated(transcript.text); });
  connect(m_dictation, &TranscriptionSession::failed, this, &QuickAIViewHost::showDictationMessage);

  m_dictationMessageTimer.setSingleShot(true);
  m_dictationMessageTimer.setInterval(DICTATION_MESSAGE_MS);
  connect(&m_dictationMessageTimer, &QTimer::timeout, this, [this]() {
    m_dictationMessage.clear();
    emit dictationMessageChanged();
  });

  connect(m_aiService, &AI::Service::modelsChanged, this, &QuickAIViewHost::updateDictationAvailable);
  connect(m_dictationService, &DictationService::settingsChanged, this,
          &QuickAIViewHost::updateDictationAvailable);
  updateDictationAvailable();
}

bool QuickAIViewHost::recording() const { return m_dictation && m_dictation->isRecording(); }

bool QuickAIViewHost::transcribing() const { return m_dictation && m_dictation->transcribing(); }

QString QuickAIViewHost::recordingTime() const {
  return m_dictation ? m_dictation->elapsedTime() : QString();
}

void QuickAIViewHost::updateDictationAvailable() {
  const bool available = m_dictationService->setup().has_value();
  if (available == m_dictationAvailable) return;
  m_dictationAvailable = available;
  emit dictationAvailableChanged();
}

void QuickAIViewHost::toggleDictation() {
  if (!m_dictation || m_dictation->transcribing()) return;
  if (m_dictation->isRecording()) {
    m_dictation->accept();
    return;
  }

  using vicinae::permissions::MicrophoneStatus;
  switch (vicinae::permissions::microphoneStatus()) {
  case MicrophoneStatus::Granted:
    startDictation();
    return;
  case MicrophoneStatus::NotDetermined:
    vicinae::permissions::requestMicrophone([this](bool granted) {
      if (granted) startDictation();
    });
    return;
  case MicrophoneStatus::Denied:
    showDictationMessage(tr("Microphone access is turned off for Vicinae"));
    return;
  }
}

void QuickAIViewHost::cancelDictation() {
  if (m_dictation) m_dictation->cancel();
}

void QuickAIViewHost::startDictation() {
  const auto setup = m_dictationService->setup();
  if (!setup) {
    showDictationMessage(tr("No transcription model selected"));
    return;
  }
  m_dictation->start(*setup);
}

void QuickAIViewHost::showDictationMessage(const QString &message) {
  m_dictationMessage = message;
  emit dictationMessageChanged();
  m_dictationMessageTimer.start();
}

void QuickAIViewHost::loadInitialData() {
  if (!m_initialQuery.isEmpty()) sendQuery(m_initialQuery.toStdString());
}

void QuickAIViewHost::send(const QString &text) {
  if (m_streaming || text.trimmed().isEmpty()) return;
  sendQuery(text.trimmed().toStdString());
}

void QuickAIViewHost::cancel() {
  if (m_stream) m_stream->abort();
}

void QuickAIViewHost::sendQuery(const std::string &query) {
  m_history.emplace_back(AI::ChatMessage::fromText(AI::ChatRole::User, query));

  m_currentResponse.clear();
  m_streamingQuery = QString::fromStdString(query);
  m_streamingContent.clear();
  m_streaming = true;
  emit streamingChanged();
  emit streamingContentChanged();

  AI::ChatCompletionPayload payload;
  payload.messages = m_history;

  m_stream = m_aiService->createChatCompletion(m_selectedModel, payload);

  if (!m_stream) {
    failQuery(tr("This model is not available right now.").toStdString());
    return;
  }

  connect(m_stream.get(), &AI::AbstractChatCompletionStream::dataAdded, this,
          [this](const std::string &text) {
            m_currentResponse += text;
            m_streamingContent = QString::fromStdString(m_currentResponse);
            emit streamingContentChanged();
          });

  connect(m_stream.get(), &AI::AbstractChatCompletionStream::finished, this, [this]() {
    // an error might have occurred, yet finished is still emitted
    if (!m_streaming) return;

    const auto &model = m_stream->model();
    m_modelLabel = QString::fromStdString(model.name);
    m_modelIcon = model.icon.value_or(ImageUrl{});
    emit modelChanged();

    m_streaming = false;
    emit streamingChanged();

    m_history.emplace_back(AI::ChatMessage::fromText(AI::ChatRole::Assistant, m_currentResponse));

    m_exchanges.append(QVariantMap{
        {QStringLiteral("query"), m_streamingQuery},
        {QStringLiteral("response"), m_streamingContent},
        {QStringLiteral("error"), QString()},
    });
    emit exchangesChanged();

    m_streamingContent.clear();
    m_streamingQuery.clear();
    emit streamingContentChanged();
    m_stream.reset();
  });

  connect(m_stream.get(), &AI::AbstractChatCompletionStream::errorOccurred, this,
          [this](const std::string &reason) { failQuery(reason); });

  m_stream->start();
}

void QuickAIViewHost::failQuery(const std::string &reason) {
  if (!m_history.empty() && m_history.back().role == AI::ChatRole::User) m_history.pop_back();

  m_exchanges.append(QVariantMap{
      {QStringLiteral("query"), m_streamingQuery},
      {QStringLiteral("response"), QString()},
      {QStringLiteral("error"), QString::fromStdString(reason)},
  });
  emit exchangesChanged();

  m_streaming = false;
  emit streamingChanged();
  m_streamingContent.clear();
  m_streamingQuery.clear();
  emit streamingContentChanged();
  m_stream.reset();
}

void QuickAIViewHost::selectModel(const QString &compositeId) {
  auto ref = AI::ModelRef::fromString(compositeId.toStdString());
  if (!ref) return;

  m_selectedModel = std::move(*ref);

  for (const auto &section : m_modelSelectorItems) {
    auto sectionMap = section.toMap();
    auto items = sectionMap[QStringLiteral("items")].toList();
    for (const auto &item : items) {
      auto itemMap = item.toMap();
      if (itemMap[QStringLiteral("id")].toString() == compositeId) {
        m_modelSelectorCurrentItem = itemMap;
        emit modelSelectorCurrentItemChanged();
        return;
      }
    }
  }
}

void QuickAIViewHost::rebuildModelSelectorItems() {
  m_modelSelectorItems = buildGroupedModelList(m_aiService, AI::Capability::Completion);
  emit modelSelectorItemsChanged();

  bool selectionStillValid = false;
  if (m_selectedModel) {
    auto compositeId = QString::fromStdString(m_selectedModel->toString());
    for (const auto &section : m_modelSelectorItems) {
      auto sectionMap = section.toMap();
      auto items = sectionMap[QStringLiteral("items")].toList();
      for (const auto &item : items) {
        auto itemMap = item.toMap();
        if (itemMap[QStringLiteral("id")].toString() == compositeId) {
          selectionStillValid = true;
          m_modelSelectorCurrentItem = itemMap;
          emit modelSelectorCurrentItemChanged();
          break;
        }
      }
      if (selectionStillValid) break;
    }
  }

  if (!selectionStillValid) {
    for (const auto &[id, provider] : m_aiService->providers()) {
      if (auto model = provider->findBestModel(AI::Capability::Completion)) {
        m_selectedModel = AI::ModelRef{provider->id(), model->id};
        auto compositeId = QString::fromStdString(m_selectedModel->toString());

        QVariantMap item;
        item[QStringLiteral("id")] = compositeId;
        item[QStringLiteral("displayName")] = QString::fromStdString(model->name);
        if (model->icon) { item[QStringLiteral("iconSource")] = model->icon->imageUrl().toString(); }
        m_modelSelectorCurrentItem = item;
        emit modelSelectorCurrentItemChanged();
        break;
      }
    }
  }
}
