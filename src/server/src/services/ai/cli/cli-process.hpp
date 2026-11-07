#pragma once
#include <optional>
#include <string>
#include <string_view>
#include <QByteArray>
#include <QObject>
#include <QProcessEnvironment>
#include <QProcess>
#include <QStringList>

namespace AI {

/**
 * Locate an agent CLI. A configured path wins, otherwise the PATH is searched, along with the install
 * locations the official installers use, which are often missing from the PATH of a desktop session.
 */
std::optional<QString> findCliExecutable(std::string_view configured, const QString &name);

/**
 * Directory the agent CLIs run from. It stays empty, so that nothing in it can leak into a conversation.
 */
QString cliWorkingDirectory();

/**
 * The system environment, with the places tools usually get installed to at the end of PATH. Apps started by
 * the desktop rather than a shell don't get what shell profiles add, such as nvm or Homebrew, and a CLI
 * installed through npm needs to find node. The directory of `executable` goes first, if there's one.
 */
QProcessEnvironment cliProcessEnvironment(const QString &executable = {});

/**
 * A child process speaking newline-delimited JSON over stdio.
 */
class JsonLineProcess : public QObject {
  Q_OBJECT

signals:
  void lineReceived(const std::string &line);
  void exited(const std::string &reason);

public:
  JsonLineProcess(const QString &program, const QStringList &arguments, QObject *parent = nullptr);

  void start();
  void write(std::string_view line);
  bool running() const { return m_process.state() != QProcess::NotRunning; }

  /**
   * Close stdin so the process exits on its own, kill it if it does not, then delete this object.
   * Nothing is emitted afterwards.
   */
  void shutdown();

private:
  static constexpr qsizetype STDERR_TAIL = 2048;
  static constexpr int SHUTDOWN_GRACE_MS = 3000;

  void readOutput();
  void handleFinished(int exitCode, QProcess::ExitStatus status);

  QProcess m_process;
  QByteArray m_buffer;
  QByteArray m_stderr;
  bool m_shuttingDown = false;
};

} // namespace AI
