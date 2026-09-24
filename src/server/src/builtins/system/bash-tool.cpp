#include <algorithm>
#include <csignal>
#include <QCoreApplication>
#include <QDir>
#include <QPointer>
#include <QProcess>
#include <QPromise>
#include <QStandardPaths>
#include <QTimer>
#include "bash-tool.hpp"

namespace {

constexpr int DEFAULT_TIMEOUT_MS = 30000;
constexpr int MAX_TIMEOUT_MS = 120000;
constexpr qsizetype MAX_OUTPUT_BYTES = 64 * 1024;

struct BashInput {
  std::string command;
  std::optional<std::string> workingDirectory;
  std::optional<int> timeoutMs;

  struct glaze {
    static constexpr auto value =
        glz::object("command", &BashInput::command, "working_directory", &BashInput::workingDirectory,
                    "timeout_ms", &BashInput::timeoutMs);
  };
};

struct BashOutput {
  std::string output;
  int exitCode;
  bool timedOut;
  bool truncated;
  std::optional<std::string> error;

  struct glaze {
    static constexpr auto value =
        glz::object("output", &BashOutput::output, "exit_code", &BashOutput::exitCode, "timed_out",
                    &BashOutput::timedOut, "truncated", &BashOutput::truncated, "error", &BashOutput::error);
  };
};

class BashRun : public QObject {
  Q_DECLARE_TR_FUNCTIONS(BashRun)

public:
  BashRun() : QObject(QCoreApplication::instance()) {
    m_promise.start();
    m_output.reserve(MAX_OUTPUT_BYTES);
    m_process.setProcessChannelMode(QProcess::MergedChannels);
#ifdef Q_OS_UNIX
    m_process.setUnixProcessParameters(QProcess::UnixProcessFlag::CreateNewSession);
#endif
    m_timeout.setSingleShot(true);
    connect(&m_timeout, &QTimer::timeout, this, [this] {
      m_timedOut = true;
      stop(tr("Command timed out."));
    });
    connect(&m_process, &QProcess::started, this, [this] {
      m_processGroup = m_process.processId();
      m_process.closeWriteChannel();
      if (m_stopReason) killProcess();
    });
    connect(&m_process, &QProcess::readyReadStandardOutput, this, &BashRun::readOutput);
    connect(&m_process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
      if (error == QProcess::FailedToStart) {
        m_stopReason = tr("Could not start Bash: %1").arg(m_process.errorString());
        finish(-1, QProcess::CrashExit);
      }
    });
    connect(&m_process, &QProcess::finished, this, &BashRun::finish);
  }

  ~BashRun() override { killProcess(); }

  AI::AbstractTool::RawToolTask start(const QString &executable, const BashInput &input) {
    auto future = m_promise.future();
    m_process.setWorkingDirectory(input.workingDirectory ? QString::fromStdString(*input.workingDirectory)
                                                         : QDir::homePath());
    m_timeout.start(input.timeoutMs.value_or(DEFAULT_TIMEOUT_MS));
    m_process.start(executable, {QStringLiteral("--noprofile"), QStringLiteral("--norc"),
                                 QStringLiteral("-c"), QString::fromStdString(input.command)});
    return {std::move(future), [run = QPointer(this)] {
              if (run) run->stop(tr("Command cancelled."));
            }};
  }

private:
  void readOutput() {
    while (m_process.bytesAvailable() > 0) {
      const auto chunk = m_process.read(8192);
      const auto remaining = MAX_OUTPUT_BYTES - m_output.size();
      m_output.append(chunk.constData(), std::min(chunk.size(), remaining));
      m_truncated |= chunk.size() > remaining;
    }
  }

  void killProcess() {
#ifdef Q_OS_UNIX
    // Bash and its children share a session, including commands in pipelines.
    if (m_processGroup > 0) {
      ::kill(-static_cast<pid_t>(m_processGroup), SIGKILL);
      m_processGroup = 0;
    }
#endif
    if (m_process.state() != QProcess::NotRunning) m_process.kill();
  }

  void stop(QString reason) {
    if (m_finished || m_stopReason) return;
    m_stopReason = std::move(reason);
    killProcess();
  }

  void finish(int exitCode, QProcess::ExitStatus status) {
    if (std::exchange(m_finished, true)) return;
    m_timeout.stop();
    readOutput();
    killProcess();
    auto display = QString::fromUtf8(m_output);
    while (display.endsWith(u'\n') || display.endsWith(u'\r'))
      display.chop(1);
    if (m_truncated) display += tr("\n[Output truncated at 64 KiB]");

    const bool failed = m_stopReason || status != QProcess::NormalExit || exitCode != 0;
    std::optional<std::string> statusText;
    if (m_stopReason)
      statusText = m_stopReason->toStdString();
    else if (status != QProcess::NormalExit)
      statusText = tr("Command crashed.").toStdString();
    else if (exitCode != 0)
      statusText = tr("Exit code %1").arg(exitCode).toStdString();

    std::string content;
    const BashOutput result{QString::fromUtf8(m_output).toStdString(), exitCode, m_timedOut, m_truncated,
                            statusText};
    if (const auto error = glz::write<JsonWriteOptions{}>(result, content)) {
      m_promise.addResult(AI::AbstractTool::RawToolResult(std::unexpected(glz::format_error(error))));
    } else {
      m_promise.addResult(AI::AbstractTool::RawToolResult(AI::ToolOutput{.content = std::move(content),
                                                                         .displayText = display.toStdString(),
                                                                         .statusText = std::move(statusText),
                                                                         .failed = failed}));
    }
    m_promise.finish();
    deleteLater();
  }

  QProcess m_process;
  QTimer m_timeout;
  QPromise<AI::AbstractTool::RawToolResult> m_promise;
  QByteArray m_output;
  std::optional<QString> m_stopReason;
  qint64 m_processGroup = 0;
  bool m_timedOut = false;
  bool m_truncated = false;
  bool m_finished = false;
};

} // namespace

std::optional<QString> BashTool::executablePath() {
#ifdef Q_OS_UNIX
  const auto path = QStandardPaths::findExecutable(QStringLiteral("bash"));
  if (!path.isEmpty()) return path;
#endif
  return std::nullopt;
}

std::string BashTool::description() const {
  return "Run a command using Bash on the user's computer. Each call starts a fresh non-interactive shell "
         "in the user's home directory unless an absolute working_directory is supplied. "
         "Use this when the request needs local command execution. Commands have the user's permissions. "
         "stdin is closed; do not start interactive or background services. "
         "Returns combined output and the exit code. Output is limited to 64 KiB. "
         "The default timeout is 30 seconds, with a maximum of 120 seconds.";
}

std::string BashTool::generateInputSchema() const {
  return R"({"type":"object","properties":{"command":{"type":"string","description":"Bash command to execute."},"working_directory":{"type":"string","description":"Absolute working directory. Defaults to the user's home directory."},"timeout_ms":{"type":"integer","minimum":1,"maximum":120000,"description":"Timeout in milliseconds. Defaults to 30000."}},"required":["command"],"additionalProperties":false})";
}

std::optional<std::string> BashTool::invocationSummary(std::string_view arguments) const {
  BashInput input;
  if (glz::read_json(input, arguments)) return std::nullopt;
  return input.command;
}

AI::AbstractTool::RawToolTask BashTool::runRaw(std::string_view arguments) {
  const auto reject = [](std::string error) -> RawToolTask {
    return {QtFuture::makeReadyValueFuture<RawToolResult>(std::unexpected(std::move(error))), {}};
  };
  BashInput input;
  if (const auto error = glz::read_json(input, arguments)) return reject(glz::format_error(error));
  if (QString::fromStdString(input.command).trimmed().isEmpty() || input.command.contains('\0'))
    return reject(tr("Enter a Bash command without NUL characters.").toStdString());
  if (input.timeoutMs && (*input.timeoutMs < 1 || *input.timeoutMs > MAX_TIMEOUT_MS))
    return reject(tr("The timeout must be between 1 and 120000 milliseconds.").toStdString());
  if (input.workingDirectory) {
    const auto path = QString::fromStdString(*input.workingDirectory);
    if (input.workingDirectory->contains('\0') || !QDir::isAbsolutePath(path) || !QDir(path).exists())
      return reject(tr("The working directory must be an existing absolute path.").toStdString());
  }
  return (new BashRun)->start(m_executable, input);
}
