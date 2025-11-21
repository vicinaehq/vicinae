#include <catch2/catch_session.hpp>
#include <QCoreApplication>

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  int result = Catch::Session().run(argc, argv);

  return result;
}
