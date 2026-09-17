#include <QGuiApplication>
#include <catch2/catch_session.hpp>

int main(int argc, char **argv) {
  Catch::Session session;
  if (const auto result = session.applyCommandLine(argc, argv); result != 0) return result;
  if (session.configData().listTests) return session.run();
  QGuiApplication app(argc, argv);
  return session.run();
}
