#pragma once
#include "omni-icon.hpp"
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
  virtual QString fullyQualifiedName() const { return name(); }
  virtual OmniIconUrl iconUrl() const = 0;
  virtual std::vector<std::shared_ptr<Application>> actions() const { return {}; }

  // whether the executable can open url(s) or file(s)
  virtual bool isOpener() { return true; }

  Application() {}
};

class AbstractAppDatabase : public QObject {
public:
  using AppPtr = std::shared_ptr<Application>;

  virtual bool launch(const Application &exec, const std::vector<QString> &args = {}) const = 0;
  virtual AppPtr findBestOpener(const QUrl &url) const = 0;
  virtual AppPtr findBestOpener(const QString &mime) const = 0;
  virtual std::vector<AppPtr> findOpeners(const QString &mime) const = 0;
  virtual AppPtr findById(const QString &id) const = 0;
  virtual std::vector<AppPtr> list() const = 0;
  virtual AppPtr findByClass(const QString &name) const = 0;

  AppPtr fileBrowser() const { return findBestOpener("inode/directory"); }
  AppPtr textEditor() const { return findBestOpener("text/plain"); }
  AppPtr webBrowser() const {
    static std::vector<QString> mimes{
        "x-scheme-handler/https", "x-scheme-handler/http", "text/html", "text/css", "text/javascript",
    };

    for (const auto &mime : mimes) {
      if (auto app = findBestOpener(mime)) return app;
    }

    return nullptr;
  }
};
