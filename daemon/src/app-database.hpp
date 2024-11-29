#pragma once
#include <qdir.h>
#include <qhash.h>
#include <qlist.h>
#include <qlogging.h>
#include <qset.h>
#include <qsettings.h>
#include <qtenvironmentvariables.h>

struct DesktopAction {
  QString id;
  QString name;
  QString exec;
};

struct DesktopFile {
  QString id;
  QString name;
  QString exec;
  QString comment;
  QString icon;
  bool noDisplay;
  QString wmClass;
  QList<QString> keywords;
  QList<QString> categories;
  QList<std::shared_ptr<DesktopFile>> actions;
};

// clang-format off
static const QList<QDir> defaultPaths = {
	QDir("/usr/share/applications"),
	QDir("/usr/local/share/applications"),
	QDir::homePath() + "/.local/share/applications"
};
// clang-format on

class AppDatabase {
  QList<QDir> paths;
  QHash<QString, std::shared_ptr<DesktopFile>> appMap;

public:
  QList<std::shared_ptr<DesktopFile>> apps;

  AppDatabase() {
    auto xdd = qgetenv("XDG_DATA_DIRS");

    if (!xdd.isEmpty()) {
      for (const auto &path : xdd.split(':')) {
        paths.push_back(QString(path) + "/applications");
      }
    } else {
      paths = defaultPaths;
    }

    QList<QDir> traversed;

    // scan dirs
    for (const auto &dir : paths) {
      if (traversed.contains(dir))
        continue;

      traversed.push_back(dir);

      if (!dir.exists())
        continue;

      for (const auto &entry : dir.entryList()) {
        if (!entry.endsWith(".desktop"))
          continue;

        QString fullpath = dir.path() + QDir::separator() + entry;
        QSettings ini(fullpath, QSettings::IniFormat);

        auto groups = ini.childGroups();
        std::shared_ptr<DesktopFile> file(nullptr);

        for (const auto &group : groups) {
          auto parts = group.split(' ');

          if (parts.size() == 2 && parts[0] == "Desktop" &&
              parts[1] == "Entry") {
            ini.beginGroup(group);
            file = std::make_shared<DesktopFile>(DesktopFile{
                .id = entry,
                .name = ini.value("Name").toString(),
                .exec = ini.value("Exec").toString(),
                .comment = ini.value("Comment").toString(),
                .icon = ini.value("Icon").toString(),
                .noDisplay = ini.value("NoDisplay").toString() == "true",
                .wmClass = ini.value("StartupWMClass").toString(),
                .keywords = ini.value("Keywords").toString().split(";"),
                .categories = ini.value("Categories").toString().split(";"),
            });
            ini.endGroup();
            break;
          }
        }

        if (!file)
          continue;

        apps.push_back(file);
        appMap.insert(fullpath, file);

        for (const auto &group : groups) {
          auto parts = group.split(' ');

          if (parts.size() == 3 && parts[0] == "Desktop" &&
              parts[1] == "Action") {
            ini.beginGroup(group);

            auto icon = ini.value("Icon", "e");

            if (icon.isNull())
              icon = file->icon;

            auto action = std::make_shared<DesktopFile>(DesktopFile{
                .id = parts[2],
                .name = ini.value("Name").toString(),
                .exec = ini.value("Exec").toString(),
                .icon = ini.value("Icon", file->icon).toString(),
            });

            file->actions.push_back(action);

            ini.endGroup();
          }
        }
      }
    }
  }
};
