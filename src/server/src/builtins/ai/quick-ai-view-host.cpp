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
    : m_initialQuery(std::move(initialQuery)), m_selectedModel(std::move(model)) {
  connect(&m_attachments, &AttachmentModel::changed, this, &QuickAIViewHost::attachmentStateChanged);
  connect(this, &QuickAIViewHost::streamingChanged, this, &QuickAIViewHost::attachmentStateChanged);
  connect(this, &QuickAIViewHost::modelSelectorCurrentItemChanged, this,
          &QuickAIViewHost::attachmentStateChanged);
}

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
  connect(m_aiService, &AI::Service::modelsChanged, this, &QuickAIViewHost::attachmentStateChanged);

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
  if (!m_initialQuery.isEmpty()) send(m_initialQuery);
}

bool QuickAIViewHost::needsVision() const {
  if (m_attachments.hasImages()) return true;
  return std::ranges::any_of(m_history, [](const AI::ChatMessage &message) {
    return std::ranges::any_of(
        message.parts, [](const AI::ChatPart &part) { return std::holds_alternative<AI::ImagePart>(part); });
  });
}

bool QuickAIViewHost::modelAcceptsImages() const {
  if (!m_aiService || !m_selectedModel) return false;
  auto *provider = m_aiService->getProviderById(m_selectedModel->provider);
  if (!provider) return false;
  const auto models = provider->listModels();
  const auto model = std::ranges::find(models, m_selectedModel->id, &AI::Model::id);
  return model != models.end() && (model->caps & AI::Capability::Vision);
}

bool QuickAIViewHost::canSend() const {
  return !m_streaming && m_attachments.ready() && (!needsVision() || modelAcceptsImages());
}

QString QuickAIViewHost::attachmentMessage() const {
  if (needsVision() && !modelAcceptsImages())
    return tr("Choose a model that supports images for this conversation.");
  return m_attachments.error();
}

bool QuickAIViewHost::send(const QString &text) {
  const auto query = text.trimmed();
  if (!canSend() || (query.isEmpty() && m_attachments.count() == 0)) return false;
  sendQuery(query.toStdString());
  return true;
}

void QuickAIViewHost::cancel() {
  if (m_stream) m_stream->abort();
}

void QuickAIViewHost::sendQuery(const std::string &query) {
  const auto previews = m_attachments.items();
  auto attachments = m_attachments.take();
  AI::ChatMessage message{.role = AI::ChatRole::User};
  message.parts.reserve(1 + attachments.size() * 2);
  if (!query.empty()) message.parts.emplace_back(AI::TextPart{query});
  for (const auto &content : attachments) {
    const auto &attachment = content.data();
    if (const auto *image = std::get_if<FileAttachment::Image>(&attachment.contents)) {
      message.parts.emplace_back(AI::TextPart{std::format("Image: {}", attachment.name)});
      message.parts.emplace_back(AI::ImagePart{image->mimeType, image->base64});
    } else {
      message.parts.emplace_back(AI::TextPart{std::format("Attached text file: {}\n\n{}", attachment.name,
                                                          std::get<std::string>(attachment.contents))});
    }
  }
  m_history.emplace_back(std::move(message));

  m_currentResponse.clear();
  m_exchanges.beginExchange(query, previews);
  m_streaming = true;
  emit streamingChanged();

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
            m_exchanges.appendResponse(text);
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

    m_exchanges.finishExchange();

    m_stream.reset();
  });

  connect(m_stream.get(), &AI::AbstractChatCompletionStream::errorOccurred, this,
          [this](const std::string &reason) { failQuery(reason); });

  m_stream->start();
}

void QuickAIViewHost::failQuery(const std::string &reason) {
  if (!m_history.empty() && m_history.back().role == AI::ChatRole::User) m_history.pop_back();

  m_exchanges.finishExchange(reason);

  m_streaming = false;
  emit streamingChanged();
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
