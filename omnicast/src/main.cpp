#include "app.hpp"
#include <QApplication>
#include <QFontDatabase>
#include <QSurfaceFormat>
#include <QtSql/QtSql>
#include <QXmlStreamReader>
#include <QtSql/qsqldatabase.h>
#include <QtWaylandClient/qwaylandclientextension.h>
#include <arpa/inet.h>
#include <cmark.h>
#include <qfontdatabase.h>
#include <qlist.h>
#include <qlocalserver.h>
#include <qlocalsocket.h>
#include <qlogging.h>
#include <qobject.h>
#include <QtWaylandClient/QWaylandClientExtension>
#include <qtmetamacros.h>
#include <wayland-util.h>

int main(int argc, char **argv) {
  QApplication qapp(argc, argv);

  AppWindow app;

  app.show();

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
