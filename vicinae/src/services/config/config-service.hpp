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

  void handleDirectoryChanged(const QString &path) {
    qDebug() << "config directory changed, reloading config...";
    reloadConfig();
  }

  void reloadConfig() {
    auto prev = m_config;
    m_config = load();
    emit configChanged(m_config, prev);
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

  void saveConfig(const Value &next) {
    QJsonDocument doc;
    QJsonObject obj;

    obj["faviconService"] = next.faviconService;
    obj["popToRootOnClose"] = next.popToRootOnClose;
    obj["closeOnFocusLoss"] = next.closeOnFocusLoss;
    obj["keybinding"] = next.keybinding;

    {
      QJsonObject binds;
      for (const auto &[k, v] : next.keybinds) {
        binds[k] = v;
      }
      obj["keybinds"] = binds;
    }

    {
      QJsonObject font;

      if (next.font.normal) { font["normal"] = *next.font.normal; }

      font["size"] = next.font.baseSize;
      obj["font"] = font;
    }

    {
      QJsonObject theme;

      if (next.theme.name) { theme["name"] = *next.theme.name; }
      if (next.theme.iconTheme) { theme["iconTheme"] = *next.theme.iconTheme; }

      obj["theme"] = theme;
    }

    {
      QJsonObject rootSearch;

      rootSearch["searchFiles"] = next.rootSearch.searchFiles;
      obj["rootSearch"] = rootSearch;
    }

    {
      QJsonObject window;

      window["rounding"] = next.window.rounding;
      window["opacity"] = next.window.opacity;
      window["csd"] = next.window.csd;
      obj["window"] = window;
    }

    QFile file(m_configFile);

    doc.setObject(obj);
    if (!file.open(QIODevice::WriteOnly)) {
      qWarning() << "Failed to open config file for writing";
      return;
    }
    file.write(doc.toJson());
    // do not emit update signal during save, wait a bit
    QTimer::singleShot(0, this, &ConfigService::reloadConfig);
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

    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, &ConfigService::handleDirectoryChanged);
  }

signals:
  void configChanged(const Value &next, const Value &prev) const;
};
