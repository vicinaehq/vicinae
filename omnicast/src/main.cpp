#include "app.hpp"
#include <QApplication>
#include <QFontDatabase>
#include <QSurfaceFormat>
#include <QtSql/QtSql>
#include <QXmlStreamReader>
#include <QtSql/qsqldatabase.h>
#include <QtWaylandClient/qwaylandclientextension.h>
#include <algorithm>
#include <arpa/inet.h>
#include <cmark.h>
#include <csignal>
#include <filesystem>
#include <fstream>
#include <qdebug.h>
#include <qfontdatabase.h>
#include <qlist.h>
#include <qlocalserver.h>
#include <qlocalsocket.h>
#include <qlogging.h>
#include <qobject.h>
#include <qprocess.h>
#include <qtmetamacros.h>
#include "omnicast.hpp"

int main(int argc, char **argv) {
  QApplication qapp(argc, argv);

  {
    std::filesystem::create_directories(Omnicast::runtimeDir());
    auto pidFile = Omnicast::runtimeDir() / "omnicast.pid";

    if (std::filesystem::exists(pidFile)) {
      int pid;
      std::ifstream ifs(pidFile);

      if (!ifs.is_open()) { qDebug() << "failed to open pid file"; }

      ifs >> pid;
      kill(pid, SIGINT);
    }

    std::ofstream ofs(pidFile);

    if (!ofs.is_open()) {
      qDebug() << "failed to open pid file for writing";
      return 1;
    }

    ofs << QApplication::applicationPid();
  }

  AppWindow app;

  app.show();

  // Print it

  int fontId = QFontDatabase::addApplicationFont(":assets/fonts/SF-Pro-Text-Regular.otf");
  fontId = QFontDatabase::addApplicationFont(":assets/fonts/SF-Pro-Text-Light.otf");
  fontId = QFontDatabase::addApplicationFont(":assets/fonts/SF-Pro-Text-Bold.otf");

  QFont font("SF Pro Text");

  font.setHintingPreference(QFont::HintingPreference::PreferNoHinting);

  QApplication::setFont(font);

  for (const auto &family : QFontDatabase::applicationFontFamilies(fontId)) {
    qDebug() << "family=" << family;
  }

  auto family = QFontDatabase::applicationFontFamilies(fontId).at(0);

  QApplication::setApplicationName("omnicast");
  QIcon::setThemeName("Tela");

  return qapp.exec();
}
