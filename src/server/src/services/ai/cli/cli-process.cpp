#include "cli-process.hpp"
#include <array>
#include <filesystem>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTimer>
#include "vicinae.hpp"

namespace AI {

namespace {

QStringList toolDirectories() {
  const auto home = QDir::homePath();
  auto dirs = QStringList{
      home + "/.local/bin",
      home + "/.claude/local",
      home + "/.npm-global/bin",
      home + "/.bun/bin",
      home + "/.volta/bin",
      home + "/.asdf/shims",
      home + "/.local/share/mise/shims",
      home + "/.local/share/fnm/aliases/default/bin",
#ifdef Q_OS_MACOS
      home + "/Library/Application Support/fnm/aliases/default/bin",
#endif
#ifdef Q_OS_WIN
      qEnvironmentVariable("APPDATA") + "/npm",
#else
      QStringLiteral("/opt/homebrew/bin"),
      QStringLiteral("/usr/local/bin"),
#endif
  };

  const QDir nvm(home + "/.nvm/versions/node");
  const auto versions = nvm.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name | QDir::Reversed);
  if (!versions.isEmpty()) dirs.emplace_back(nvm.filePath(versions.front() + "/bin"));

  return dirs;
}

} // namespace

QProcessEnvironment cliProcessEnvironment(const QString &executable) {
  auto environment = QProcessEnvironment::systemEnvironment();
  auto path = environment.value(QStringLiteral("PATH")).split(QDir::listSeparator(), Qt::SkipEmptyParts);

  for (const auto &dir : toolDirectories()) {
    if (!path.contains(dir) && QFileInfo(dir).isDir()) path.append(dir);
  }
  if (!executable.isEmpty()) path.prepend(QFileInfo(executable).absolutePath());

  environment.insert(QStringLiteral("PATH"), path.join(QDir::listSeparator()));
  return environment;
}

std::optional<QString> findCliExecutable(std::string_view configured, const QString &name) {
  if (!configured.empty()) {
    auto path = QString::fromUtf8(configured.data(), static_cast<qsizetype>(configured.size()));
    if (path.startsWith(QStringLiteral("~/"))) path = QDir::homePath() + path.mid(1);
    if (QFileInfo info(path); info.isFile() && info.isExecutable()) return info.absoluteFilePath();
    return std::nullopt;
  }

  if (auto path = QStandardPaths::findExecutable(name); !path.isEmpty()) return path;

  if (auto path = QStandardPaths::findExecutable(name, toolDirectories()); !path.isEmpty()) return path;

  return std::nullopt;
}

QString cliWorkingDirectory() {
  const auto dir = Omnicast::cacheDir() / "ai-cli";
  std::error_code ec;
  std::filesystem::create_directories(dir, ec);
  return QString::fromStdString(dir.string());
}

JsonLineProcess::JsonLineProcess(const QString &program, const QStringList &arguments, QObject *parent)
    : QObject(parent) {
  m_process.setProgram(program);
  m_process.setArguments(arguments);
  m_process.setWorkingDirectory(cliWorkingDirectory());
  m_process.setProcessEnvironment(cliProcessEnvironment(program));

  connect(&m_process, &QProcess::readyReadStandardOutput, this, &JsonLineProcess::readOutput);
  connect(&m_process, &QProcess::readyReadStandardError, this, [this] {
    m_stderr.append(m_process.readAllStandardError());
    if (m_stderr.size() > STDERR_TAIL) m_stderr = m_stderr.right(STDERR_TAIL);
  });
  connect(&m_process, &QProcess::finished, this, &JsonLineProcess::handleFinished);
  connect(&m_process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
    if (error != QProcess::FailedToStart || m_shuttingDown) return;
    emit exited(std::format("Could not start {}: {}", m_process.program().toStdString(),
                            m_process.errorString().toStdString()));
  });
}

void JsonLineProcess::unsetEnvironment(const QString &name) {
  auto environment = m_process.processEnvironment();
  environment.remove(name);
  m_process.setProcessEnvironment(environment);
}

void JsonLineProcess::start() { m_process.start(QIODevice::ReadWrite); }

void JsonLineProcess::write(std::string_view line) {
  m_process.write(line.data(), static_cast<qint64>(line.size()));
  m_process.write("\n", 1);
}

void JsonLineProcess::shutdown() {
  if (m_shuttingDown) return;
  m_shuttingDown = true;
  setParent(nullptr);

  if (!running()) {
    deleteLater();
    return;
  }

  m_process.closeWriteChannel();
  QTimer::singleShot(SHUTDOWN_GRACE_MS, this, [this] {
    if (running()) m_process.kill();
  });
}

void JsonLineProcess::readOutput() {
  m_buffer.append(m_process.readAllStandardOutput());

  qsizetype start = 0;
  for (auto end = m_buffer.indexOf('\n'); end != -1; end = m_buffer.indexOf('\n', start)) {
    const auto line = QByteArrayView(m_buffer).sliced(start, end - start).trimmed();
    start = end + 1;
    if (line.isEmpty() || m_shuttingDown) continue;
    emit lineReceived(std::string(line.data(), static_cast<std::size_t>(line.size())));
  }

  m_buffer.remove(0, start);
}

void JsonLineProcess::handleFinished(int exitCode, QProcess::ExitStatus status) {
  if (m_shuttingDown) {
    deleteLater();
    return;
  }

  auto reason = QString::fromUtf8(m_stderr).trimmed().toStdString();
  if (reason.empty()) {
    reason = status == QProcess::CrashExit ? "The process crashed."
                                           : std::format("The process exited with code {}.", exitCode);
  }
  emit exited(reason);
}

} // namespace AI
