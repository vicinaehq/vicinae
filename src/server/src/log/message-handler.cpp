#include "message-handler.hpp"
#include "rang/rang.hpp"

void coloredMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  QString const timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd'T'hh:mm:ss");
  rang::fg color;
  std::string_view levelName;

  switch (type) {
  case QtDebugMsg:
    color = rang::fg::cyan;
    levelName = "debug";
    break;
  case QtInfoMsg:
    color = rang::fg::green;
    levelName = "info ";
    break;
  case QtWarningMsg:
    color = rang::fg::yellow;
    levelName = "warn ";
    break;
  case QtCriticalMsg:
    color = rang::fg::red;
    levelName = "error";
    break;
  case QtFatalMsg:
    color = rang::fg::magenta;
    levelName = "FATAL";
    break;
  }

  std::cerr << rang::fg::reset << "[" << rang::fg::yellow << "V" << rang::fg::reset << "] " << rang::fg::gray
            << timestamp.toStdString() << " " << color << levelName << rang::fg::reset << " - "
            << msg.toStdString();

  if (context.file) {
    std::filesystem::path const file(context.file);
    std::cerr << " (" << rang::fg::blue << file.filename().string() << ":" << context.line << rang::fg::reset
              << ")";
  }

  std::cerr << "\n";

  if (type == QtFatalMsg) { abort(); }
}
