#pragma once
#include "../../ui/image/url.hpp"
#include <QString>
#include <qmimetype.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qurl.h>
#include <quuid.h>

class Application {
public:
  virtual QString id() const = 0;
  virtual QString name() const = 0;
  virtual bool displayable() const = 0;
  virtual bool isTerminalApp() const = 0;
  virtual bool isTerminalEmulator() const { return false; }
  virtual QString fullyQualifiedName() const { return name(); }
  virtual ImageURL iconUrl() const = 0;
  virtual std::vector<std::shared_ptr<Application>> actions() const { return {}; }
  virtual std::vector<QString> keywords() const { return {}; }
  virtual std::filesystem::path path() const = 0;

  /**
   * Current version of the installed application (if applicable)
   * The empty string is interpreted as no version being available
   */
  virtual QString version() const { return {}; }

  /**
   * A short multiline description that explains what the app does.
   */
  virtual QString description() const = 0;

  // whether the executable can open url(s) or file(s)
  virtual bool isOpener() { return true; }

  Application() {}
};

class AbstractAppDatabase : public QObject {
public:
  using AppPtr = std::shared_ptr<Application>;

  virtual std::vector<std::filesystem::path> defaultSearchPaths() const = 0;

  virtual bool scan(const std::vector<std::filesystem::path> &paths) = 0;

  virtual bool launch(const Application &exec, const std::vector<QString> &args = {}) const = 0;
  /**
   * Returns the best app to open the passed target.
   * Target can be any string, including an URL or a simple filename/path.
   * It is up to the implementation to properly handle those different target
   * formats.
   */
  virtual AppPtr findBestOpener(const QString &target) const = 0;
  virtual AppPtr findBestOpenerForMime(const QString &target) const = 0;
  virtual std::vector<AppPtr> findOpeners(const QString &mime) const = 0;
  virtual AppPtr findById(const QString &id) const = 0;
  virtual std::vector<AppPtr> list() const = 0;
  virtual AppPtr findByClass(const QString &name) const = 0;

  AppPtr fileBrowser() const { return findBestOpenerForMime("inode/directory"); }
  AppPtr textEditor() const { return findBestOpenerForMime("text/plain"); }
  AppPtr webBrowser() const {
    static std::vector<QString> mimes{
        "x-scheme-handler/https", "x-scheme-handler/http", "text/html", "text/css", "text/javascript",
    };

    return findBestOpener("https://example.com");
  }
};
