#include <QDir>
#include <QCoreApplication>
#include <QPointer>
#include <QProcess>
#include <QPromise>
#include <QStandardPaths>
#include <QTimer>
#include <algorithm>
#include <optional>
#ifdef Q_OS_UNIX
#include <csignal>
#endif
#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif
#include "bash-tool.hpp"

namespace AI {
struct BashInput {
  std::string command;
  std::optional<std::string> working_directory;

  struct glaze_json_schema {
    glz::schema command{.description = "Bash command to execute"};
    glz::schema working_directory{.description =
                                      "Absolute working directory; defaults to the user's home directory"};
  };
};

namespace {
constexpr qsizetype MAX_OUTPUT_BYTES = 64 * 1024;
constexpr int TIMEOUT_MS = 30000;

class BashInvocation : public QObject {
  Q_DECLARE_TR_FUNCTIONS(BashInvocation)

public:
  BashInvocation(QString program, const BashInput &input) : QObject(QCoreApplication::instance()) {
    m_promise.start();
    m_process.setProgram(std::move(program));
    m_process.setProcessChannelMode(QProcess::MergedChannels);
    m_process.setArguments({QStringLiteral("--noprofile"), QStringLiteral("--norc"), QStringLiteral("-c"),
                            QString::fromStdString(input.command)});
    m_process.setWorkingDirectory(input.working_directory ? QString::fromStdString(*input.working_directory)
                                                          : QDir::homePath());
#ifdef Q_OS_UNIX
    m_process.setUnixProcessParameters(QProcess::UnixProcessFlag::CreateNewSession);
#endif
    connect(&m_process, &QProcess::started, this, [this] {
      m_pid = m_process.processId();
#ifdef Q_OS_WIN
      m_job = CreateJobObjectW(nullptr, nullptr);
      JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};
      limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
      const auto process = OpenProcess(PROCESS_SET_QUOTA | PROCESS_TERMINATE, FALSE, DWORD(m_pid));
      const bool attached =
          m_job && process &&
          SetInformationJobObject(m_job, JobObjectExtendedLimitInformation, &limits, sizeof(limits)) &&
          AssignProcessToJobObject(m_job, process);
      if (process) CloseHandle(process);
      if (!attached) stop(tr("Could not manage the Bash process.").toStdString());
#endif
      if (m_stopReason) killProcesses();
    });
    connect(&m_process, &QProcess::readyReadStandardOutput, this, [this] { readOutput(); });
    connect(&m_process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
      if (error == QProcess::FailedToStart) finish(-1, m_process.errorString().toStdString());
    });
    connect(&m_process, &QProcess::finished, this, [this](int code, QProcess::ExitStatus status) {
      finish(code, m_stopReason                    ? m_stopReason
                   : status == QProcess::CrashExit ? std::optional(tr("Process terminated.").toStdString())
                                                   : std::nullopt);
    });
    m_timeout.setSingleShot(true);
    connect(&m_timeout, &QTimer::timeout, this,
            [this] { stop(tr("Timed out after 30 seconds").toStdString()); });
  }

  ~BashInvocation() override {
    if (!m_finished) killProcesses();
  }

  AbstractTool::RawToolTask start() {
    auto future = m_promise.future();
    QTimer::singleShot(0, this, [this] {
      if (m_stopReason) {
        finish(-1, m_stopReason);
        return;
      }
      m_timeout.start(TIMEOUT_MS);
      m_process.start();
      m_process.closeWriteChannel();
    });
    return {std::move(future), [guard = QPointer(this)] {
              if (guard) guard->stop(tr("Command cancelled.").toStdString());
            }};
  }

private:
  void readOutput() {
    const auto data = m_process.readAllStandardOutput();
    const auto remaining = std::max(qsizetype(0), MAX_OUTPUT_BYTES - m_output.size());
    m_output.append(data.first(std::min(remaining, data.size())));
    m_truncated |= data.size() > remaining;
  }

  void killProcesses() {
#ifdef Q_OS_UNIX
    if (m_pid > 0) ::kill(-m_pid, SIGKILL);
#endif
#ifdef Q_OS_WIN
    if (m_job) {
      CloseHandle(m_job);
      m_job = nullptr;
    }
#endif
    if (m_process.state() != QProcess::NotRunning) m_process.kill();
  }

  void stop(std::string reason) {
    if (m_finished) return;
    m_stopReason = std::move(reason);
    killProcesses();
  }

  void finish(int code, std::optional<std::string> error = {}) {
    if (std::exchange(m_finished, true)) return;
    m_timeout.stop();
    readOutput();
    killProcesses();
    auto output = QString::fromUtf8(m_output).toStdString();
    if (m_truncated) output += "\n" + tr("Output truncated at 64 KiB").toStdString();
    auto content = std::format("exit_code: {}\n{}", code, output);
    if (error) content += "\n" + *error;
    auto status = error;
    if (!status && code != 0) status = tr("Exited with code %1").arg(code).toStdString();
    m_promise.addResult(ToolOutput{.content = std::move(content),
                                   .displayText = std::move(output),
                                   .statusText = std::move(status),
                                   .failed = error.has_value() || code != 0});
    m_promise.finish();
    deleteLater();
  }

  QProcess m_process;
  QTimer m_timeout;
  QPromise<AbstractTool::RawToolResult> m_promise;
  QByteArray m_output;
  std::optional<std::string> m_stopReason;
  qint64 m_pid = 0;
  bool m_truncated = false;
  bool m_finished = false;
#ifdef Q_OS_WIN
  HANDLE m_job = nullptr;
#endif
};
} // namespace

bool BashTool::available() { return !QStandardPaths::findExecutable(QStringLiteral("bash")).isEmpty(); }

std::string BashTool::description() const {
  return "Run a Bash command on the user's computer. Commands are non-interactive, limited to 30 seconds, "
         "and return an exit code and combined output (up to 64 KiB).";
}

std::string BashTool::generateInputSchema() const {
  std::string schema;
  [[maybe_unused]] const auto error = glz::write_json_schema<BashInput>(schema);
  return schema;
}

std::optional<std::string> BashTool::invocationSummary(std::string_view arguments) const {
  BashInput input;
  if (glz::read_json(input, arguments) || input.command.empty()) return {};
  return std::move(input.command);
}

AbstractTool::RawToolTask BashTool::runRaw(std::string_view arguments) {
  const auto fail = [](std::string reason) -> RawToolTask {
    return {QtFuture::makeReadyValueFuture<RawToolResult>(std::unexpected(std::move(reason))), {}};
  };
  BashInput input;
  if (const auto error = glz::read_json(input, arguments)) return fail(glz::format_error(error));
  if (input.command.empty()) return fail("The command is empty.");
  if (input.working_directory && !QDir::isAbsolutePath(QString::fromStdString(*input.working_directory)))
    return fail("The working directory must be an absolute path.");
  const auto program = QStandardPaths::findExecutable(QStringLiteral("bash"));
  if (program.isEmpty()) return fail("Bash is not installed.");
  return (new BashInvocation(program, input))->start();
}

} // namespace AI
