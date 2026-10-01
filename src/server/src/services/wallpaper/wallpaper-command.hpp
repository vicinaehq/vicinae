#pragma once
#include <expected>
#include <string>
#include <QProcess>
#include <QString>
#include <QStringList>

namespace wallpaper {

// Run a command synchronously, returning stdout or a human-readable error. The timeout is only a
// ceiling (we return as soon as the process exits), so it stays generous for slow applies (large gifs).
inline std::expected<QByteArray, std::string> runCommand(const QString &program, const QStringList &args,
                                                         int timeoutMs = 30000) {
  QProcess proc;

  proc.start(program, args);

  if (!proc.waitForStarted(timeoutMs)) {
    return std::unexpected(std::string{"failed to start "} + program.toStdString() + ": " +
                           proc.errorString().toStdString());
  }

  if (!proc.waitForFinished(timeoutMs)) {
    proc.kill();
    return std::unexpected(program.toStdString() + " timed out");
  }

  if (proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
    auto err = QString::fromUtf8(proc.readAllStandardError()).trimmed();
    if (err.isEmpty()) { err = QString{"%1 exited with code %2"}.arg(program).arg(proc.exitCode()); }
    return std::unexpected(err.toStdString());
  }

  return proc.readAllStandardOutput();
}

} // namespace wallpaper
