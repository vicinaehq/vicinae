#include "app.hpp"
#include <QApplication>
#include <QFontDatabase>
#include <QSurfaceFormat>
#include <jsoncpp/json/reader.h>
#include <qfontdatabase.h>
#include <qlogging.h>

static const char *colorSelected = "rgba(43, 42, 41, 230)";
static const char *baseBackground = "rgba(30, 29, 28, 240)";
static const char *baseText = "rgb(239, 239, 239)";

static const char *defaultStyleSheet = R"(
QWidget {
	font-family: '%1';
	font-size: 10pt;
	font-weight: lighter;
	letter-spacing: -0.5px;
	color: %4;
}

QListWidget {
	background-color: %2;
	border: none;
}

#CommandWidget {
	background-color: %2;
}

QLabel.minor {
	color: #AAAAAA;
}

QListWidget::item {
	font-size: 9pt;
	margin-left: 8px;
	border-radius: 8px;
	margin-right: 8px;
}

QListWidget, .top-bar QLineEdit, .quicklink-completion {
	background-color: %2;
}

QListWidget {
	border-bottom: 1px white solid;
}

.quicklink-completion QLineEdit {
	background-color: %3;
	border: 1px #ffffff solid;
	font-size: 11pt;
	border-radius: 5px;
}

.top-bar > QLineEdit {
	font-size: 12pt;
	border: none;
}

QListWidget::item:hover, QListWidget::item:selected {
	background-color: %3;
}

QLabel.transform-left {
	font-size: 16pt;
	font-weight: bold;
}

QLabel.chip {
	background-color: %2;
	border-radius: 5px;
}

QWidget.status-bar {
	background-color: %3;
}

.status-bar QLabel {
	font-size: 9pt;
}
)";

int main(int argc, char **argv) {
  QApplication qapp(argc, argv);
  auto app = new AppWindow();

  app->show();

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
                         .arg(baseText));

  return qapp.exec();
}
