#pragma once
#include <QDebug>
#include <filesystem>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <qfilesystemwatcher.h>
#include <qlogging.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <string>
#include <unordered_map>
#include <variant>

namespace AI {

struct ConfigValue {
  struct ModelConfig {
    bool enabled = true;
  };

  struct ProviderConfigBase {
    std::string id;
    std::string type;
  };

  struct OllamaConfig {
    std::string type;
    std::string url = "http://localhost:11434";
  };

  /*
  struct OpenAIConfig {
    std::string type;
    std::string apiKey;
  };
  */

  struct MistralConfig {
    std::string type;
    std::string apiKey;
  };

  using ProviderConfig = std::variant<OllamaConfig, MistralConfig>;

  std::unordered_map<std::string, ProviderConfig> providers;
  std::unordered_map<std::string, ModelConfig> models;
};

struct ProviderTypeInfo {
  std::string_view type;
  std::string_view label;
  std::string_view icon;
  std::string_view description;
  bool allowMultiple;

  ConfigValue::ProviderConfig makeDefault() const {
    if (type == "ollama") return ConfigValue::OllamaConfig{.type = "ollama"};
    if (type == "mistral") return ConfigValue::MistralConfig{.type = "mistral"};
    return ConfigValue::OllamaConfig{};
  }
};

// NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays)
inline constexpr ProviderTypeInfo kProviderTypes[] = {
    {
        .type = "ollama",
        .label = "Ollama",
        .icon = "ollama",
        .description = "Connect to a local or remote Ollama instance.",
        .allowMultiple = true,
    },
    {
        .type = "mistral",
        .label = "Mistral",
        .icon = "mistral",
        .description = "Mistral AI cloud API. Provides transcription and language models.",
        .allowMultiple = false,
    },
};
// NOLINTEND(cppcoreguidelines-avoid-c-arrays)

inline const ProviderTypeInfo *findProviderType(std::string_view type) {
  for (const auto &info : kProviderTypes) {
    if (info.type == type) return &info;
  }
  return nullptr;
}

class ConfigManager : public QObject {
  Q_OBJECT

signals:
  void configChanged() const;

public:
  ConfigManager(std::filesystem::path path) : m_path(std::move(path)) {
    m_watcher.addPath(QString::fromStdString(m_path.parent_path()));
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, [this]() { reloadConfig(); });
  }

  const ConfigValue &value() const { return m_value; }

  ConfigValue &value() { return m_value; }

  void load() { reloadConfig(); }

  bool save() {
    static thread_local std::string buf;
    std::filesystem::create_directories(m_path.parent_path());
    if (auto const error = glz::write_file_json(m_value, m_path.c_str(), buf)) {
      qWarning() << "Failed to save AI config:" << glz::format_error(error);
      return false;
    }
    emit configChanged();
    return true;
  }

private:
  void reloadConfig() {
    auto const result = loadConfig();
    if (!result) {
      qWarning() << "Failed to load AI config" << result.error();
      return;
    }
    m_value = result.value();
    emit configChanged();
  }

  std::expected<ConfigValue, std::string> loadConfig() {
    static thread_local std::string buf;
    ConfigValue value;

    qDebug() << "load from" << m_path.c_str();
    std::filesystem::create_directories(m_path.parent_path());

    if (auto const error = glz::read_file_jsonc(value, m_path.c_str(), buf)) {
      return std::unexpected(glz::format_error(error));
    }

    return value;
  }

private:
  QFileSystemWatcher m_watcher;
  std::filesystem::path m_path;
  ConfigValue m_value;
};
}; // namespace AI
