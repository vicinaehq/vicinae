#include <QCoreApplication>
#include <catch2/catch_session.hpp>
#ifdef VICINAE_TEST_GUI
#include <QGuiApplication>
#endif

int main(int argc, char **argv) {
  Catch::Session session;
  if (const auto result = session.applyCommandLine(argc, argv); result != 0) return result;
  if (session.configData().listTests) return session.run();
#ifdef VICINAE_TEST_GUI
  QGuiApplication app(argc, argv);
#else
  QCoreApplication app(argc, argv);
#endif
  return session.run();
}
