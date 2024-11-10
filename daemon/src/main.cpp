#include "app.hpp"
#include <QApplication>
#include <QSurfaceFormat>
#include <jsoncpp/json/reader.h>

int main(int argc, char **argv) {
  QApplication qapp(argc, argv);
  auto app = new AppWindow();

  app->show();

  return qapp.exec();
}
