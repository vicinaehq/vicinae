#include "app.hpp"
#include <QApplication>
#include <jsoncpp/json/reader.h>

int main(int argc, char **argv) {
  QApplication qapp(argc, argv);
  AppWindow app;

  app.show();

  return qapp.exec();
}
