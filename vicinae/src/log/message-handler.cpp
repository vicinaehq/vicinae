#include "message-handler.hpp"

void coloredMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  // ANSI color codes
  const char *BLACK = "\033[30m";
  const char *RED = "\033[31m";
  const char *GREEN = "\033[32m";
  const char *YELLOW = "\033[33m";
  const char *BLUE = "\033[34m";
  const char *MAGENTA = "\033[35m";
  const char *CYAN = "\033[36m";
  const char *WHITE = "\033[37m";
  const char *RESET = "\033[0m";

  QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
  QString contextInfo = "";

  if (context.file) {
    std::filesystem::path file(context.file);

    contextInfo = QString("(%1%2:%3%4)").arg(BLUE).arg(file.filename().c_str()).arg(context.line).arg(RESET);
  }

  QString color;
  QString levelName;

  switch (type) {
  case QtDebugMsg:
    color = CYAN;
    levelName = "DEBUG";
    break;
  case QtInfoMsg:
    color = GREEN;
    levelName = "INFO ";
    break;
  case QtWarningMsg:
    color = YELLOW;
    levelName = "WARN ";
    break;
  case QtCriticalMsg:
    color = RED;
    levelName = "ERROR";
    break;
  case QtFatalMsg:
    color = MAGENTA;
    levelName = "FATAL";
    break;
  }

  // Format: [time] LEVEL message (file:line)
  QString formattedMessage = QString("%1[%2] %3%4%5  -  %6 %7%8\n")
                                 .arg(WHITE)
                                 .arg(timestamp)
                                 .arg(color)
                                 .arg(levelName)
                                 .arg(RESET)
                                 .arg(msg)
                                 .arg(contextInfo)
                                 .arg(RESET);

  std::cerr << formattedMessage.toStdString();

  if (type == QtFatalMsg) { abort(); }
}
