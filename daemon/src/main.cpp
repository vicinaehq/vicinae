#include "app.hpp"
#include <QApplication>
#include <QFontDatabase>
#include <QSurfaceFormat>
#include <QtSql/QtSql>
#include <QtSql/qsqldatabase.h>
#include <jsoncpp/json/reader.h>
#include <qfontdatabase.h>
#include <qlist.h>
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

QMainWindow > QWidget {
	background-color: %2;
}

QLineEdit, QListWidget {
	border: none;
	background-color: transparent;
}

.managed-list::item {
	font-size: 9pt;
	margin-left: 8px;
	border-radius: 8px;
	margin-right: 8px;
}

#action-popover {
	background-color: %5;
	border-radius: 5px;
	background-color: rgba(35, 35, 35, 240);
}

#action-popover QLineEdit {
	background-color: transparent;
	border: none;
}

#action-popover QListWidget {
	background-color: transparent;
	border-bottom: 1px solid #444;
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

QWidget.status-bar {
	background-color: %5;
}

.status-bar QLabel {
	font-size: 9pt;
}
)";

int main(int argc, char **argv) {
  QApplication qapp(argc, argv);
  AppWindow app;

  app.show();

  int fontId = QFontDatabase::addApplicationFont(
      "./assets/fonts/SF-Pro-Text-Regular.otf");
  fontId =
      QFontDatabase::addApplicationFont("./assets/fonts/SF-Pro-Text-Light.otf");
  fontId =
      QFontDatabase::addApplicationFont("./assets/fonts/SF-Pro-Text-Bold.otf");

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

  return qapp.exec();
}
