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
#include <csignal>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
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

void coloredMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  // ANSI color codes
  const char *BLACK = "\033[30m";
  const char *RED = "\033[31m";
  const char *GREEN = "\033[32m";
  const char *YELLOW = "\033[33m";
  const char *BLUE = "\033[34m";
  const char *MAGENTA = "\033[35m";
  const char *CYAN = "\033[36m";
  const char *WHITE = "\033[37m";
  const char *RESET = "\033[0m";

  QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
  QString contextInfo = "";

  if (context.file) {
    std::filesystem::path file(context.file);

    contextInfo = QString("(%1%2:%3%4)").arg(BLUE).arg(file.filename().c_str()).arg(context.line).arg(RESET);
  }

  QString color;
  QString levelName;

  switch (type) {
  case QtDebugMsg:
    color = CYAN;
    levelName = "debug";
    break;
  case QtInfoMsg:
    color = GREEN;
    levelName = "info ";
    break;
  case QtWarningMsg:
    color = YELLOW;
    levelName = "warn ";
    break;
  case QtCriticalMsg:
    color = RED;
    levelName = "error";
    break;
  case QtFatalMsg:
    color = MAGENTA;
    levelName = "fatal";
    break;
  }

  // Format: [time] LEVEL message (file:line)
  QString formattedMessage = QString("%1[%2] %3%4%5  -  %6 %7%8\n")
                                 .arg(WHITE)
                                 .arg(timestamp)
                                 .arg(color)
                                 .arg(levelName)
                                 .arg(RESET)
                                 .arg(msg)
                                 .arg(contextInfo)
                                 .arg(RESET);

  std::cerr << formattedMessage.toStdString();

  if (type == QtFatalMsg) { abort(); }
}

int startDaemon() {
  std::filesystem::create_directories(Omnicast::runtimeDir());
  auto pidFile = Omnicast::runtimeDir() / "omnicast.pid";

  {
    std::ifstream ifs(pidFile);
    pid_t pid;

    if (ifs.is_open()) {
      ifs >> pid;

      qDebug() << "Kill existing omnicast instance with pid" << pid;

      if (kill(pid, SIGKILL) < 0) {
        qDebug() << "Failed to kill existing omnicast instance with pid" << pid << strerror(errno);
      }
    }
  }

  {
    std::ofstream ofs(pidFile);

    if (!ofs.is_open()) {
      qDebug() << "failed to open pid file for writing";
      return 1;
    }

    ofs << qApp->applicationPid();
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

  return qApp->exec();
}

int main(int argc, char **argv) {
  QApplication qapp(argc, argv);

  qInstallMessageHandler(coloredMessageHandler);

  if (qapp.arguments().size() == 2 && qapp.arguments().at(1) == "server") { return startDaemon(); }

  QLocalSocket socket;
  Proto::Marshaler marshaler;

  socket.connectToServer(Omnicast::commandSocketPath().c_str());

  if (!socket.waitForConnected(1000)) {
    qDebug() << "Failed to connect to omnicast daemon. Is omnicast running? You can start a new omnicast "
                "instance by runnning 'omnicast server'";
    return 1;
  }

  Proto::Array args{"ping", {}};
  auto packet = marshaler.marshalSized(args);

  socket.write(reinterpret_cast<const char *>(packet.data()), packet.size());
  qDebug() << "Ping sent to omnicast daemon";

  if (!socket.waitForBytesWritten(1000)) {
    qDebug() << "Failed to connect to omnicast daemon. Is omnicast running? You can start a new omnicast "
                "instance by runnning 'omnicast server'";
    return 1;
  }

  if (!socket.waitForReadyRead(1000)) {
    qDebug() << "Failed to connect to omnicast daemon. Is omnicast running? You can start a new omnicast "
                "instance by runnning 'omnicast server'";
    return 1;
  }

  socket.readAll();

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
    Proto::Array args{"url-scheme-handler", argv[1]};
    auto packet = marshaler.marshalSized(args);

    qDebug() << "Handling URL" << url;
    socket.write(reinterpret_cast<const char *>(packet.data()), packet.size());
    socket.waitForBytesWritten();
    return 0;
  }
}
