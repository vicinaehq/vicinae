#include "message-handler.hpp"
#include "rang/rang.hpp"
#include <format>
#include <fstream>
#include <mutex>
#include <qlogging.h>

namespace fs = std::filesystem;

namespace {

constexpr auto MAX_LOG_SIZE = 5 * 1024 * 1024;

struct LevelStyle {
  rang::fg color = rang::fg::reset;
  std::string_view name;
};

rang::fg sourceColor(std::string_view source) {
  if (source == vicinae::log::FILE_INDEXER) return rang::fg::blue;
  if (source == vicinae::log::EXTENSION) return rang::fg::magenta;
  if (source == vicinae::log::INPUT_SERVER) return rang::fg::cyan;
  if (source == vicinae::log::CLIPBOARD_SERVER) return rang::fg::green;
  return rang::fg::yellow;
}

std::string timestamp() {
  return QDateTime::currentDateTime().toString("yyyy-MM-dd'T'hh:mm:ss").toStdString();
}

LevelStyle levelStyle(QStringView name) {
  if (name == u"error" || name == u"fatal") return {rang::fg::red, "error"};
  if (name == u"warn" || name == u"warning") return {rang::fg::yellow, "warn "};
  if (name == u"debug" || name == u"trace") return {rang::fg::cyan, "debug"};
  return {rang::fg::green, "info "};
}

LevelStyle levelStyle(QtMsgType type) {
  switch (type) {
  case QtDebugMsg:
    return {rang::fg::cyan, "debug"};
  case QtInfoMsg:
    return {rang::fg::green, "info "};
  case QtWarningMsg:
    return {rang::fg::yellow, "warn "};
  case QtCriticalMsg:
    return {rang::fg::red, "error"};
  case QtFatalMsg:
    return {rang::fg::magenta, "FATAL"};
  }

  return {};
}

class MessageSink {
public:
  static MessageSink &instance() {
    static MessageSink sink;
    return sink;
  }

  void openFile(const fs::path &path) {
    std::lock_guard const lock(m_mutex);
    std::error_code ec;

    m_path = path;
    m_rotatedPath = path.parent_path() / (path.filename().string() + ".1");
    fs::create_directories(m_path.parent_path(), ec);
    m_file.open(m_path, std::ios::out | std::ios::app);

    auto const size = fs::file_size(m_path, ec);
    m_size = ec ? 0 : size;
  }

  void write(std::string_view source, const LevelStyle &style, std::string_view timestamp,
             std::string_view message, std::string_view location) {
    std::lock_guard const lock(m_mutex);

    std::cerr << rang::fg::reset << "[" << sourceColor(source) << source << rang::fg::reset << "] "
              << rang::fg::gray << timestamp << " " << style.color << style.name << rang::fg::reset << " - "
              << message;

    if (!location.empty()) { std::cerr << " (" << rang::fg::blue << location << rang::fg::reset << ")"; }

    std::cerr << "\n";

    auto line = std::format("[{}] {} {} - {}", source, timestamp, style.name, message);

    if (!location.empty()) { line += std::format(" ({})", location); }

    line += '\n';
    writeToFile(line);
  }

private:
  void writeToFile(std::string_view line) {
    if (!m_file.is_open()) return;

    m_file.write(line.data(), line.size());
    m_file.flush();
    m_size += line.size();

    if (m_size >= MAX_LOG_SIZE) { rotate(); }
  }

  void rotate() {
    m_file.close();

    std::error_code ec;
    fs::rename(m_path, m_rotatedPath, ec);

    m_file.open(m_path, std::ios::out | std::ios::trunc);
    m_size = 0;
  }

  std::mutex m_mutex;
  fs::path m_path;
  fs::path m_rotatedPath;
  std::ofstream m_file;
  std::uintmax_t m_size = 0;
};

void coloredMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  std::string location;

  if (context.file) {
    fs::path const file(context.file);
    location = std::format("{}:{}", file.filename().string(), context.line);
  }

  MessageSink::instance().write(vicinae::log::SERVER, levelStyle(type), timestamp(), msg.toStdString(),
                                location);

  if (type == QtFatalMsg) { abort(); }
}

} // namespace

namespace vicinae::log {

void installMessageHandler() { qInstallMessageHandler(coloredMessageHandler); }

void openFile(const std::filesystem::path &path) { MessageSink::instance().openFile(path); }

void subprocessLine(std::string_view source, QStringView level, QStringView message) {
  auto const trimmed = message.trimmed();

  if (trimmed.isEmpty()) return;

#if defined(QT_NO_DEBUG_OUTPUT)
  if (level == "debug") return;
#endif

  MessageSink::instance().write(source, levelStyle(level), timestamp(), trimmed.toString().toStdString(), {});
}

} // namespace vicinae::log
