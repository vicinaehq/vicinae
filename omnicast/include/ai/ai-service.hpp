#pragma once
#include "ai/ai-provider.hpp"
#include "omni-database.hpp"
#include "omni-icon.hpp"
#include <algorithm>
#include <qdebug.h>
#include <qsqlerror.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlogging.h>
#include <qobject.h>
#include <qsqlquery.h>

class BaseView;

namespace AI {

class AbstractProvider : public QObject {

public:
  virtual QString id() const = 0;
  virtual QString displayName() const = 0;
  virtual bool isAlive() const = 0;

  /**
   * Bundled icons only: `:icons/<iconName>.svg`
   */
  virtual QString iconName() const { return "stars"; }

  OmniIconUrl iconUrl() const { return BuiltinOmniIconUrl(iconName()); }

  virtual std::vector<AiModel> listModels() const = 0;

  virtual std::optional<AiModel> findBestForTask(AiTaskType type) const = 0;

  /**
   * Creates a chat completion.
   * If streaming is not enabled, the `StreamedChatCompletion` object is expected to only fire one event
   * when the completion has been fully generated.
   */
  virtual StreamedChatCompletion *createCompletion(const CompletionPayload &payload) const = 0;

  /**
   * Loads or reloads the configuration of this provider.
   * Returns whether the configuration information is valid or not.
   */
  virtual bool loadConfig(const QJsonObject &value) = 0;

  /**
   * The configuration view to show to the user when they
   * want to configure the provider.
   * The view is responsible for calling the AI manager and updating
   * the configuration on submission.
   */
  virtual BaseView *configView() = 0;
};

class Manager {
  OmniDatabase &m_db;

public:
  struct ManagedProvider {
    struct Config {
      bool enabled;
      QJsonObject data;
    };

    std::unique_ptr<AbstractProvider> provider;
    int priority;
    bool configured;
    bool enabled;
  };

private:
  std::vector<ManagedProvider> m_providers;

public:
  Manager(OmniDatabase &db) : m_db(db) {}

  std::optional<ManagedProvider::Config> getProviderConfig(const QString &id) const {
    QSqlQuery query(m_db.db());

    query.prepare("SELECT data, enabled FROM ai_provider_config WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
      qCritical() << "Failed to getProviderConfig:" << query.lastError();
      return std::nullopt;
    }

    if (!query.next()) { return std::nullopt; }

    QJsonParseError parseError;
    auto json = QJsonDocument::fromJson(query.value(0).toByteArray(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
      qCritical() << "getProviderConfig: failed to parse json";
      return std::nullopt;
    }

    return ManagedProvider::Config{.enabled = query.value(1).toBool(), .data = json.object()};
  }

  ManagedProvider *findManagedProvider(const QString &id) {
    for (auto &managed : m_providers) {
      if (id == managed.provider->id()) return &managed;
    }

    return nullptr;
  }

  bool setProviderConfig(const QString &id, const QJsonObject &data) {
    QSqlQuery query(m_db.db());
    QJsonDocument doc(data);

    query.prepare(R"(
		INSERT INTO ai_provider_config (id, data) VALUES (:id, :data)
		ON CONFLICT(id) DO UPDATE SET data = :data, updated_at = (unixepoch())
	)");
    query.bindValue(":id", id);
    query.bindValue(":data", doc.toJson(QJsonDocument::JsonFormat::Compact));

    if (!query.exec()) {
      qDebug() << "Failed to set provider config" << query.lastError();
      return false;
    }

    if (auto managed = findManagedProvider(id)) {
      managed->provider->loadConfig(data);
      managed->configured = true;
    }

    return true;
  }

  bool setProviderEnabled(const QString &id, bool value) {
    QSqlQuery query(m_db.db());

    query.prepare("UPDATE ai_provider_config SET enabled = :enabled WHERE :id = :id");
    query.bindValue(":enabled", value);
    query.bindValue(":id", id);

    if (!query.exec()) {
      qCritical() << "setProviderEnabled: failed to update value" << query.lastError();
      return false;
    }

    if (auto provider = findManagedProvider(id)) { provider->enabled = value; }

    return true;
  }

  bool isAvailable() const {
    for (const auto &managed : m_providers) {
      if (managed.configured && managed.enabled && managed.provider->isAlive()) return true;
    }

    return false;
  }

  const auto &providers() const { return m_providers; }

  bool enableProvider(const QString &id) { return setProviderEnabled(id, true); }

  bool disableProvider(const QString &id) { return setProviderEnabled(id, false); }

  void unregisterProvider(const QString &id) {
    auto _ = std::remove_if(m_providers.begin(), m_providers.end(),
                            [id](const auto &managed) { return managed.provider->id() == id; });
  }

  bool registerProvider(std::unique_ptr<AbstractProvider> provider, int priority = 10) {
    unregisterProvider(provider->id());

    ManagedProvider managed;

    managed.priority = priority;

    if (auto config = getProviderConfig(provider->id())) {
      managed.configured = true;
      managed.enabled = config->enabled;
      provider->loadConfig(config->data);
    } else {
      managed.configured = false;
      managed.enabled = false;
    }

    managed.provider = std::move(provider);
    m_providers.emplace_back(std::move(managed));
    std::sort(m_providers.begin(), m_providers.end(),
              [](const ManagedProvider &a, const ManagedProvider &b) { return a.priority < b.priority; });

    return true;
  }

  std::optional<AiModel> findBestModelForTask(AiTaskType type) const {
    for (const auto &entry : m_providers) {
      if (auto model = entry.provider->findBestForTask(type)) { return model; }
    }

    return std::nullopt;
  }

  std::vector<AiModel> listModels() {
    std::vector<AiModel> models;

    models.reserve(10);

    for (const auto &entry : m_providers) {
      auto entryModels = entry.provider->listModels();

      models.insert(models.end(), entryModels.begin(), entryModels.end());
    }

    return models;
  }

  StreamedChatCompletion *createCompletion(const QString &prompt, const QString &modelId = "") {
    return createCompletion({ChatMessage(prompt)}, modelId);
  }

  StreamedChatCompletion *createCompletion(const std::vector<ChatMessage> &context,
                                           const QString &modelId = "") {
    qDebug() << "requesting completion for" << modelId;
    for (const auto &entry : m_providers) {
      // if (!entry.enabled || !entry.configured) continue;

      if (!entry.provider->isAlive()) {
        qWarning() << "Not considering AI provider" << entry.provider->id() << "isAlive() returned false";
        continue;
      }

      if (modelId.isEmpty()) {
        qDebug() << "finding best for" << entry.provider->id();
        if (auto best = entry.provider->findBestForTask(AiTaskType::QuickReasoningTask)) {
          return entry.provider->createCompletion({.modelId = best->id, .messages = context});
        }

        continue;
      }

      for (const auto &model : entry.provider->listModels()) {
        if (model.id == modelId) {
          return entry.provider->createCompletion({.modelId = modelId, .messages = context});
        }
      }
    }

    return nullptr;
  }
};

}; // namespace AI
