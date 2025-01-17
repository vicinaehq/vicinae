#include "app.hpp"
#include <QApplication>
#include <QFontDatabase>
#include <QSurfaceFormat>
#include <QtSql/QtSql>
#include <QtSql/qsqldatabase.h>
#include <arpa/inet.h>
#include <cmark.h>
#include <jsoncpp/json/reader.h>
#include <qfontdatabase.h>
#include <qlist.h>
#include <qlocalserver.h>
#include <qlocalsocket.h>
#include <qlogging.h>
#include <qobject.h>

static const char *colorSelected = "rgba(40, 39, 38, 255)";
// static const char *colorSelected = "rgba(53, 52, 51, 255)";
//  static const char *baseBackground = "rgba(30, 29, 28, 240)";
static const char *baseBackground = "rgba(23, 22, 21, 240)";
static const char *baseText = "rgb(239, 239, 239)";
static const char *statusBackground = "rgba(35, 35, 35, 255)";

static const char *defaultStyleSheet = R"(
QWidget {
	font-family: '%1';
	font-size: 10pt;
	font-weight: lighter;
	letter-spacing: -0.5px;
	color: %4;
}

/*
QMainWindow > QWidget, .virtual-list {
	background-color: %2;
}
*/

QLineEdit, QListWidget {
	border: none;
	background-color: transparent;
}

#action-popover {
	border: none;
	border-radius: 5px;
	border: 1px white solid;
	background-color: rgba(23, 22, 21, 200)
}

#action-popover QLineEdit {
	background-color: transparent;
	border: none;
}

#action-popover QListWidget {
	background-color: transparent;
	border-bottom: 1px solid #444;
}

#action-popover QListWidget::item {
	font-size: 9pt;
	border-radius: 8px;
}

QScrollArea, QScrollArea > QWidget { background: transparent; }

#fucking-viewport { background: transparent; }

VirtualListItemWidget {
	background-color: transparent;
}

VirtualListItemWidget[selected="true"] {
	background-color: %3;
}

VirtualListItemWidget[hovered="true"] {
	background-color: %3;
}

QListWidget::item:hover, 
QListWidget::item:selected {
	background-color: %3;
}


QLabel.minor {
	color: #AAAAAA;
}

QLabel.transform-left {
	font-size: 16pt;
	font-weight: bold;
}

QLabel.chip {
	background-color: %2;
	border-radius: 5px;
}

.top-bar > QLineEdit {
	font-size: 12pt;
}

.top-bar .back-button {
	background-color: %3;
	border-radius: 5px;
	padding: 5px;
}

.quicklink-completion QLineEdit {
	background-color: %3;
	border: 1px white solid;
	font-size: 11pt;
	border-radius: 5px;
}

.status-bar QLabel {
	font-size: 9pt;
}

.details-row {
	border: none;
	border-bottom: 1px solid #666666;
}

.divider {
	color: #222222;
}

QLineEdit[isFormQLineEdit="true"]  {
	padding: 8px;
	border: none;
	border: 1px solid #222222;
	border-radius: 5px;
}


QLineEdit[isFormQLineEdit="true"]:focus  {
	border: none;
	border: 1px solid #666666;
}

.popover {
	background-color: %2;
	border: none;
	border: 1px solid #222222;
}

QTextEdit {
	background-color: transparent;
}
)";

int main(int argc, char **argv) {
  QApplication qapp(argc, argv);
  QString socketPath = QDir::temp().absoluteFilePath("spellcastd.sock");
  QFile socketFile(socketPath);

  if (socketFile.exists()) {
    QLocalSocket client;

    client.connectToServer(socketPath);
    if (client.waitForConnected()) {
      QByteArray packet;
      quint32 length = 4;
      QDataStream stream(&packet, QIODevice::WriteOnly);

      stream.setByteOrder(QDataStream::BigEndian);
      stream << length;
      stream << "Open";
      qDebug() << "Opening running instance";

      client.write("open");
      client.flush();
      client.waitForDisconnected();

      return 0;
    }

    qDebug() << "Failed to connect, will attempt to launch app again...";
  }

  socketFile.remove();

  QLocalServer server;

  AppWindow app;

  app.show();

  int fontId = QFontDatabase::addApplicationFont(":assets/fonts/SF-Pro-Text-Regular.otf");
  fontId = QFontDatabase::addApplicationFont(":assets/fonts/SF-Pro-Text-Light.otf");
  fontId = QFontDatabase::addApplicationFont(":assets/fonts/SF-Pro-Text-Bold.otf");

  for (const auto &family : QFontDatabase::applicationFontFamilies(fontId)) {
    qDebug() << "family=" << family;
  }

  auto family = QFontDatabase::applicationFontFamilies(fontId).at(0);

  QApplication::setApplicationName("spellcast");
  QIcon::setThemeName("Papirus-Dark");

  qapp.setStyleSheet(QString(defaultStyleSheet)
                         .arg(family)
                         .arg(baseBackground)
                         .arg(colorSelected)
                         .arg(baseText)
                         .arg(statusBackground));

  if (!server.listen(socketPath)) {
    qDebug() << "Local server could not listen on " << socketPath;
    return 1;
  }

  qDebug() << "Server listening on " << socketPath;

  QObject::connect(&server, &QLocalServer::newConnection, [&server, &app]() {
    QLocalSocket *socket = server.nextPendingConnection();

    socket->waitForReadyRead();
    auto data = socket->readAll();

    qDebug() << data;

    if (data == "open") {
      if (app.isHidden())
        app.show();
      else
        app.hide();
    }

    socket->write("ok");
    socket->disconnectFromServer();
  });

  return qapp.exec();
}
