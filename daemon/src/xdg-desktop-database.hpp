#pragma once
#include "index-command.hpp"
#include "ini.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <qcontainerfwd.h>
#include <qdebug.h>
#include <qlogging.h>
#include <system_error>
#include <vector>

class App : public IActionnable {
public:
  std::string normalizedName;
  std::string name;
  std::string exec;
  std::string icon;
  std::string mime;

  using Ref = const App &;

  struct Open : public IAction {
    Ref ref;

    QString name() const override { return "Open Application"; }

    void exec(const QList<QString> cmd) const override {
      qDebug() << "executng app ";
    }

    Open(Ref ref) : ref(ref) {}
  };

  ActionList generateActions() const override {
    return {std::make_shared<Open>(*this)};
  }
};

class XdgDesktopDatabase {
  std::vector<App *> apps;

public:
  XdgDesktopDatabase() {
    const char *xdd = getenv("XDG_DATA_DIRS");

    if (!xdd)
      return;

    auto ss = SplitIterator(xdd, ":");

    while (auto s = ss.next()) {
      auto path = std::filesystem::path(*s) / "applications";
      std::error_code ec;

      for (const auto &entry : std::filesystem::directory_iterator(path, ec)) {
        if (ec)
          continue;

        std::string_view view(entry.path().c_str());

        if (!view.ends_with(".desktop") || view.starts_with('.')) {
          std::cerr << "Skipping " << view
                    << " because it is missing the .desktop extension"
                    << std::endl;
          continue;
        }

        std::ifstream ifs(view.data());

        if (!ifs) {
          std::cout << "Could not open " << view << std::endl;
          continue;
        }

        Ini::Parser parser(ifs);
        auto app = new App();

        parser.onKey([app](std::string_view section, std::string_view key,
                           std::string_view value) {
          if (section == "Desktop Entry") {
            if (key == "Name")
              app->name = value;
            if (key == "Exec")
              app->exec = value;
            if (key == "Icon")
              app->icon = value;
            if (key == "MimeType")
              app->mime = value;
          }
        });

        parser.parse();
        app->normalizedName = app->name;
        std::transform(app->name.begin(), app->name.end(),
                       app->normalizedName.begin(), ::tolower);

        if (app->mime.find("scheme-handler") != std::string::npos) {
          qDebug() << "scheme handler in " << app->mime;
        }

        this->apps.push_back(app);
      }
    }
  }

  std::vector<App> query(const std::string &qq) {
    std::vector<App> results;
    int count = 0;

    std::string q(qq);
    std::transform(qq.begin(), qq.end(), q.begin(), ::tolower);

    for (const auto &app : apps) {
      if (app->normalizedName.find(q) != std::string::npos) {
        results.push_back(*app);
        ++count;
      }

      if (count > 30)
        break;
    }

    return results;
  }
};
