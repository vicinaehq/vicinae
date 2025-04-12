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
#include <qbuffer.h>
#include <qdebug.h>
#include <qfontdatabase.h>
#include <qlist.h>
#include <qlocalserver.h>
#include <qlocalsocket.h>
#include <qlogging.h>
#include <qobject.h>
#include <qprocess.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include "omnicast.hpp"
#include "proto.hpp"

int startDaemon(int argc, char **argv) {
  auto pidFile = Omnicast::runtimeDir() / "omnicast.pid";
  QApplication qapp(argc, argv);

  {
    std::ofstream ofs(pidFile);

    if (!ofs.is_open()) {
      qDebug() << "failed to open pid file for writing";
      return 1;
    }

    ofs << qapp.applicationPid();
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

  auto family = QFontDatabase::applicationFontFamilies(fontId).at(0);

  QApplication::setApplicationName("omnicast");
  QIcon::setThemeName("Tela");

  return qapp.exec();
}

int main(int argc, char **argv) {
  std::filesystem::create_directories(Omnicast::runtimeDir());

  if (!std::filesystem::exists(Omnicast::pidFile())) { return startDaemon(argc, argv); }

  QLocalSocket socket;
  Proto::Marshaler marshaler;

  socket.connectToServer(Omnicast::commandSocketPath().c_str());

  if (!socket.waitForConnected(3000)) { return startDaemon(argc, argv); }

  qDebug() << "connected";
  Proto::Array args{"ping", {}};
  auto packet = marshaler.marshalSized(args);

  socket.write(reinterpret_cast<const char *>(packet.data()), packet.size());

  if (!socket.waitForBytesWritten(3000)) { return startDaemon(argc, argv); }

  qDebug() << "wait for ready read";
  socket.waitForReadyRead();
  qDebug() << "Waited";
  socket.readAll();

  qDebug() << "socket connected" << (socket.state() == QLocalSocket::ConnectedState);

  if (argc == 1) {
    Proto::Array args{"toggle", {}};
    auto packet = marshaler.marshalSized(args);

    qDebug() << "Opening running instance";
    socket.write(reinterpret_cast<const char *>(packet.data()), packet.size());
    socket.waitForBytesWritten();
    return 0;
  }

  QUrl url(argv[1]);

  if (url.isValid()) {
    if (url.scheme() != "omnicast") {
      qDebug() << "Unsupported URL scheme" << url.scheme();
      return 1;
    }

    Proto::Array args{"url-scheme-handler", argv[1]};
    auto packet = marshaler.marshalSized(args);

    qDebug() << "Handling URL" << url;
    socket.write(reinterpret_cast<const char *>(packet.data()), packet.size());
    socket.waitForBytesWritten();
    return 0;
  }

  qDebug() << "Usage:" << argv[0] << "[url]";
  return 0;
}
