#pragma once
#include "services/app-service/abstract-app-db.hpp"
#include "ui/image/url.hpp"
#include <xdgpp/desktop-entry/file.hpp>
#include <qfileinfo.h>
#include <qlogging.h>
#include <qmimedatabase.h>
#include <qmimetype.h>
#include <qobjectdefs.h>
#include <qprocess.h>
#include <xdgpp/xdgpp.hpp>

class XdgApplicationBase : public AbstractApplication {
public:
  virtual std::vector<QString> exec() const = 0;
  QString program() const override {
    auto ss = exec();
    return ss.empty() ? QString() : ss.at(0);
  }
};

/*
class XdgApplicationAction : public XdgApplicationBase {
  QString _id;
  xdgpp::DesktopEntryAction m_data;
  xdgpp::DesktopEntry m_parentData;
  QString m_parentPath;

  ImageURL iconUrl() const override {
    auto icon = m_data.icon().value_or(m_parentData.icon().value_or(""));
    return ImageURL::system(icon.c_str());
  }

  QString description() const override { return m_parentData.comment().value_or("").c_str(); }

  std::vector<QString> exec() const override { return {}; }

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
*/

class XdgApplication : public XdgApplicationBase {
  xdgpp::DesktopFile m_entry;

  QString simplifiedId() const { return id().remove(".desktop"); }

public:
  QString id() const override { return QString::fromStdString(std::string(m_entry.id())); }
  QString displayName() const override { return QString::fromStdString(m_entry.name()); }

  const xdgpp::DesktopFile &data() const { return m_entry; }

  bool displayable() const override { return m_entry.shouldBeShownInCurrentContext(); }

  bool isTerminalApp() const override { return m_entry.terminal(); }
  // bool isTerminalEmulator() const override { return _data.categories.contains("TerminalEmulator"); }
  //
  bool isTerminalEmulator() const override { return false; }
  std::filesystem::path path() const override { return m_entry.path(); };
  QString description() const override { return QString::fromStdString(m_entry.comment().value_or("")); }

  std::vector<QString> windowClasses() const override {
    std::vector<QString> classes;

    if (auto wmClass = m_entry.startupWMClass()) {
      classes.emplace_back(QString::fromStdString(wmClass.value()));
    }

    classes.emplace_back(program());
    classes.emplace_back(simplifiedId());

    return classes;
  }

  std::vector<QString> keywords() const override {
    std::vector<QString> strs;

    for (const auto &kw : m_entry.keywords()) {
      strs.emplace_back(QString::fromStdString(kw));
    }

    return strs;
  }

  ImageURL iconUrl() const override {
    auto icon = QString::fromStdString(m_entry.icon().value_or(""));
    return ImageURL::system(icon);
  }
  std::vector<std::shared_ptr<AbstractApplication>> actions() const override {
    /*
std::vector<std::shared_ptr<Application>> apps;

for (const auto &action : _data.actions) {
apps.emplace_back(std::make_shared<XdgApplicationAction>(action, _data, _path, id()));
}

return apps;
  */
    return {};
  }

  std::vector<QString> exec() const override {
    std::vector<QString> texec;

    for (const auto &s : m_entry.parseExec()) {
      texec.emplace_back(QString::fromStdString(s));
    }

    return texec;
  }

  XdgApplication(const xdgpp::DesktopFile &file) : m_entry(file) {}
};

class XdgAppDatabase : public AbstractAppDatabase {
public:
  bool scan(const std::vector<std::filesystem::path> &paths) override;
  std::vector<std::filesystem::path> defaultSearchPaths() const override;
  AppPtr findByClass(const QString &name) const override;
  AppPtr findDefaultOpener(const QString &target) const override;
  std::vector<AppPtr> findOpeners(const QString &mime) const override;
  AppPtr findById(const QString &id) const override;
  std::vector<AppPtr> list() const override;
  std::vector<AppPtr> findOpeners(const QString &mimeName);
  bool launch(const AbstractApplication &exec, const std::vector<QString> &args = {}) const override;

  XdgAppDatabase();

private:
  AppPtr defaultForMime(const QString &mime) const;
  std::vector<AppPtr> findAssociations(const QString &mime) const;
  QString mimeNameForTarget(const QString &target) const;
  AppPtr findBestTerminalEmulator() const;

  std::unordered_map<QString, std::shared_ptr<AbstractApplication>> appMap;
  std::vector<xdgpp::MimeAppsListFile> m_mimeAppsLists;
  QMimeDatabase m_mimeDb;
  std::vector<std::shared_ptr<XdgApplication>> m_apps;

  // apps segmented by data dir (needed for association resolution)
  std::unordered_map<std::filesystem::path, std::vector<std::shared_ptr<XdgApplication>>> m_dataDirToApps;
};
