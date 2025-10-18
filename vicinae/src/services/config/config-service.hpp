#pragma once
#include <qdir.h>
#include <qfilesystemwatcher.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlogging.h>
#include <qobject.h>
#include <qstring.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <optional>
#include <filesystem>
#include "vicinae.hpp"

// XXX - Currently we store the config file on the filesystem as a json file
// There is no guarantee that this will remain the case, and the config file should not be edited
// by hand if possible.
class ConfigService : public QObject {
  Q_OBJECT

public:
  struct Value {
    QString faviconService = Omnicast::DEFAULT_FAVICON_SERVICE;
    bool popToRootOnClose = true;
    bool closeOnFocusLoss = false;
    QString keybinding = "default";
    struct {
      std::optional<QString> name;
      std::optional<QString> iconTheme;
    } theme;
    struct {
      int rounding = 10;
      double opacity = 0.98;
      bool csd = true;
    } window;
    struct {
      bool searchFiles;
    } rootSearch;
    struct {
      std::optional<QString> normal;
      double baseSize = 10.5;
    } font;
    std::unordered_map<QString, QString> keybinds;
  };

private:
  QFileSystemWatcher m_watcher;
  Value m_config;
  std::filesystem::path m_configFile = Omnicast::configDir() / "vicinae.json";

  QJsonObject loadAsJson() const {
    QFile file(m_configFile);

    if (!file.open(QIODevice::ReadOnly)) { return {}; }

    return QJsonDocument::fromJson(file.readAll()).object();
  }

  Value load() const {
    QJsonObject obj = loadAsJson();
    Value cfg;

    cfg.faviconService = obj.value("faviconService").toString("google");
    cfg.popToRootOnClose = obj.value("popToRootOnClose").toBool(true);
    cfg.closeOnFocusLoss = obj.value("closeOnFocusLoss").toBool(false);
    cfg.keybinding = obj.value("keybinding").toString("default");

    {
      auto font = obj.value("font").toObject();

      if (font.contains("normal")) { cfg.font.normal = font.value("normal").toString(); }

      cfg.font.baseSize = font.value("size").toDouble(10.5);
    }

    {
      auto keybinds = obj.value("keybinds").toObject();

      for (const auto &key : keybinds.keys()) {
        cfg.keybinds[key] = keybinds.value(key).toString();
      }
    }

    {
      auto theme = obj.value("theme").toObject();

      cfg.theme.name = theme.value("name").toString("vicinae-dark");
      if (theme.contains("iconTheme")) { cfg.theme.iconTheme = theme.value("iconTheme").toString(); }
    }

    {
      auto rootSearch = obj.value("rootSearch").toObject();

      cfg.rootSearch.searchFiles = rootSearch.value("searchFiles").toBool(true);
    }

    {
      auto window = obj.value("window").toObject();

      cfg.window.rounding = window.value("rounding").toInt(10);
      cfg.window.opacity = window.value("opacity").toDouble(0.98);
      cfg.window.csd = window.value("csd").toBool(true);
    }

    return cfg;
  }

  void handleDirectoryChanged(const QString &path) {}

  void handleFileChanged(const QString &path) {
    if (path != m_configFile.c_str()) return;

    auto prev = m_config;

    m_config = load();
    emit configChanged(m_config, prev);
    m_watcher.addPath(path);
  }

public:
  void reload() { m_config = load(); }

  const Value &value() const { return m_config; }

  /**
   * Simulates a config change without persisting it, useful for live previewing config changes.
   */
  void previewConfig(const Value &config) { emit configChanged(config, m_config); }

  void updatePreviewConfig(const std::function<void(Value &value)> &updater) {
    Value newValue = m_config;

    updater(newValue);
    previewConfig(newValue);
  }

  void updateConfig(const std::function<void(Value &value)> &updater) {
    Value newValue = m_config;

    updater(newValue);
    saveConfig(newValue);
  }

  void saveConfig(const Value &value) {
    QJsonDocument doc;
    QJsonObject obj;

    obj["faviconService"] = value.faviconService;
    obj["popToRootOnClose"] = value.popToRootOnClose;
    obj["closeOnFocusLoss"] = value.closeOnFocusLoss;
    obj["keybinding"] = value.keybinding;

    {
      QJsonObject binds;
      for (const auto &[k, v] : value.keybinds) {
        binds[k] = v;
      }
      obj["keybinds"] = binds;
    }

    {
      QJsonObject font;

      if (value.font.normal) { font["normal"] = *value.font.normal; }

      font["size"] = value.font.baseSize;
      obj["font"] = font;
    }

    {
      QJsonObject theme;

      if (value.theme.name) { theme["name"] = *value.theme.name; }
      if (value.theme.iconTheme) { theme["iconTheme"] = *value.theme.iconTheme; }

      obj["theme"] = theme;
    }

    {
      QJsonObject rootSearch;

      rootSearch["searchFiles"] = value.rootSearch.searchFiles;
      obj["rootSearch"] = rootSearch;
    }

    {
      QJsonObject window;

      window["rounding"] = value.window.rounding;
      window["opacity"] = value.window.opacity;
      window["csd"] = value.window.csd;
      obj["window"] = window;
    }

    QFile file(m_configFile);

    if (!file.open(QIODevice::WriteOnly)) { return; }

    doc.setObject(obj);
    m_watcher.blockSignals(true);
    file.write(doc.toJson());
    emit configChanged(value, m_config);
    m_config = value;
    m_watcher.blockSignals(false);
  }

  ConfigService() {
    std::filesystem::create_directories(Omnicast::configDir());

    {
      QFile file(m_configFile);

      if (!file.exists()) { saveConfig({}); }
    }

    m_config = load();

    if (!m_watcher.addPath(Omnicast::configDir().c_str())) {
      qWarning() << "Failed to watch config directory";
    }

    if (!m_watcher.addPath(m_configFile.c_str())) {
      qWarning() << "Failed to watch config file at" << m_configFile;
    }

    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, &ConfigService::handleDirectoryChanged);
    connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, &ConfigService::handleFileChanged);
  }

signals:
  void configChanged(const Value &next, const Value &prev) const;
};
