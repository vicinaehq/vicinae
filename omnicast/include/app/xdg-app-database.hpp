#pragma once
#include "app/app-database.hpp"
#include "omni-icon.hpp"
#include "xdg/xdg-desktop.hpp"
#include <qfileinfo.h>
#include <qmimedatabase.h>
#include <qmimetype.h>
#include <qprocess.h>
#include <set>
#include <ranges>

class XdgApplicationBase : public Application {
public:
  virtual std::vector<QString> exec() const = 0;
};

class XdgApplicationAction : public XdgApplicationBase {
  QString _id;
  XdgDesktopEntry::Action _data;
  XdgDesktopEntry _parentData;
  QString m_parentPath;

  OmniIconUrl iconUrl() const override {
    return _data.icon.isEmpty() ? SystemOmniIconUrl(_parentData.icon) : SystemOmniIconUrl(_data.icon);
  }

  QString description() const override { return _parentData.comment; }

  std::vector<QString> exec() const override { return {_data.exec.begin(), _data.exec.end()}; }
  bool displayable() const override { return !_parentData.noDisplay; }
  bool isTerminalApp() const override { return _parentData.terminal; }
  QString fullyQualifiedName() const override { return _parentData.name + ": " + _data.name; }
  QString name() const override { return _data.name; }
  QString id() const override { return _id; };
  std::filesystem::path path() const override { return m_parentPath.toStdString(); };
  QString version() const override { return _parentData.version; }

public:
  XdgApplicationAction(const XdgDesktopEntry::Action &action, const XdgDesktopEntry &parentData,
                       const QString &parentPath, const QString &parentId)
      : _id(parentId + "." + action.id), _data(action), _parentData(parentData), m_parentPath(parentPath) {}
};

class XdgApplication : public XdgApplicationBase {
  QString _path;
  QString _id;
  XdgDesktopEntry _data;

public:
  const XdgDesktopEntry &xdgData() const { return _data; }
  QString id() const override { return _id; }
  QString name() const override { return _data.name; }
  bool displayable() const override { return !_data.noDisplay; }
  bool isTerminalApp() const override { return _data.terminal; }
  bool isTerminalEmulator() const override { return _data.categories.contains("TerminalEmulator"); }
  std::filesystem::path path() const override { return _path.toStdString(); };
  QString description() const override { return _data.comment; }
  QString version() const override { return _data.version; }

  std::vector<QString> keywords() const override { return {_data.keywords.begin(), _data.keywords.end()}; }
  OmniIconUrl iconUrl() const override { return SystemOmniIconUrl(_data.icon); }
  std::vector<std::shared_ptr<Application>> actions() const override {
    auto makeAction = [&](const XdgDesktopEntry::Action action) -> std::shared_ptr<Application> {
      return std::shared_ptr<Application>(new XdgApplicationAction(action, _data, _path, id()));
    };

    return _data.actions | std::views::transform(makeAction) | std::ranges::to<std::vector>();
  }

  std::vector<QString> exec() const override { return {_data.exec.begin(), _data.exec.end()}; }

  XdgApplication(const QFileInfo &info, const XdgDesktopEntry &data)
      : _path(info.filePath()), _id(info.fileName()), _data(data) {}
};

class XdgAppDatabase : public AbstractAppDatabase {
  std::vector<QDir> paths;
  std::unordered_map<QString, std::shared_ptr<Application>> appMap;
  std::unordered_map<QString, std::set<QString>> mimeToApps;
  std::unordered_map<QString, std::set<QString>> appToMimes;
  std::unordered_map<QString, QString> mimeToDefaultApp;
  QMimeDatabase mimeDb;
  std::vector<std::shared_ptr<XdgApplication>> apps;

  std::shared_ptr<Application> defaultForMime(const QString &mime) const;
  bool addDesktopFile(const QString &path);

public:
  std::vector<std::filesystem::path> defaultSearchPaths() const override;
  AppPtr findByClass(const QString &name) const override;
  AppPtr findBestOpener(const QString &target) const override;
  AppPtr findBestOpenerForMime(const QString &target) const override;
  std::vector<AppPtr> findOpeners(const QString &mime) const override;
  AppPtr findById(const QString &id) const override;
  std::vector<AppPtr> list() const override;
  std::vector<AppPtr> findOpeners(const QString &mimeName);
  bool launch(const Application &exec, const std::vector<QString> &args = {}) const override;

  XdgAppDatabase();
};
