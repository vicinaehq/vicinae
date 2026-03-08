#pragma once
#include <QDebug>
#include <filesystem>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
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

  void load() { reloadConfig(); }

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
