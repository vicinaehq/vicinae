#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char **argv) {
  QGuiApplication app(argc, argv);
  QGuiApplication::setApplicationName("QML Document");
  QQmlApplicationEngine engine;
  QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app,
                   [] { QCoreApplication::exit(1); });
  engine.loadFromModule("DocumentExample", "Main");
  return app.exec();
}
