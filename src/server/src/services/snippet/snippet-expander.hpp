#pragma once
#include <optional>
#include <QDateTime>
#include <QGuiApplication>
#include <QProcess>
#include <QtConcurrent>
#include <qclipboard.h>
#include <qcontainerfwd.h>
#include <quuid.h>
#include <ranges>
#include "common/types.hpp"
#include "placeholder.hpp"

class SnippetExpander {
public:
  struct ResultPart {
    QString text;
    bool placeholder = false;
  };

  struct Result {
    std::vector<ResultPart> parts;
    std::optional<int> cursorPos;
  };

  struct Options {
    // shell placeholders are expanded to $(code) if set to false. We typically use this
    // in previews, since they are done on the fly as the user searches and we don't want to risk doing
    // expensive operations (and potentially blocking, see `executeShellPlaceholdersSync`).
    bool executeShell = true;
  };

  SnippetExpander() : m_uuid(QUuid::createUuid().toString(QUuid::WithoutBraces)) {}

  QString expandToString(const QString &text,
                         const std::vector<std::pair<QString, QString>> &arguments) const {
    const auto expanded = expand(text, arguments);

    return expanded.parts | std::views::transform([](auto &&pair) { return pair.text; }) | std::views::join |
           std::ranges::to<QString>();
  }

  Result expand(const QString &text, const std::vector<std::pair<QString, QString>> &arguments) const {
    return expand(text, arguments, Options{});
  }

  Result expand(const QString &text, const std::vector<std::pair<QString, QString>> &arguments,
                const Options &opts) const {
    const auto clip = QGuiApplication::clipboard();
    const auto parsed = PlaceholderString::parseSnippetText(text);

    const auto shellResults =
        opts.executeShell ? executeShellPlaceholdersSync(parsed) : std::vector<QString>{};
    int shellIdx = 0;
    Result result;

    for (const auto &part : parsed.parts()) {
      part |
          match{[&](const PlaceholderString::ParsedPlaceholder placeholder) {
                  if (placeholder.id == "cursor") {
                    result.cursorPos = std::ranges::fold_left(
                        result.parts, 0, [](int n, auto &&s) { return n + s.text.size(); });
                  } else if (placeholder.id == "clipboard") {
                    result.parts.emplace_back(clip->text(), true);
                  } else if (placeholder.id == "uuid") {
                    result.parts.emplace_back(ResultPart(m_uuid, true));
                  } else if (placeholder.id == "date") {
                    auto fmt = QStringLiteral("yyyy-MM-dd hh:mm");
                    if (auto it = placeholder.args.find("format"); it != placeholder.args.end())
                      fmt = it->second;
                    result.parts.emplace_back(ResultPart(QDateTime::currentDateTime().toString(fmt), true));
                  } else if (placeholder.id == "shell") {
                    if (opts.executeShell && shellIdx < static_cast<int>(shellResults.size())) {
                      result.parts.emplace_back(ResultPart(shellResults[shellIdx], true));
                    } else {
                      auto code = placeholder.args.contains("code") ? placeholder.args.at("code") : QString();
                      result.parts.emplace_back(ResultPart(QStringLiteral("$(%1)").arg(code), true));
                    }
                    ++shellIdx;
                  } else if (placeholder.id == "argument") {
                    if (const auto it = placeholder.args.find("name"); it != placeholder.args.end()) {
                      if (const auto it2 = std::ranges::find_if(
                              arguments, [&](auto &&pair) { return pair.first == it->second; });
                          it2 != arguments.end()) {
                        result.parts.emplace_back(it2->second);
                      }
                    } else {
                      qDebug() << "no name!";
                    }
                  } else {
                    if (const auto it2 = std::ranges::find_if(
                            arguments, [&](auto &&pair) { return pair.first == placeholder.id; });
                        it2 != arguments.end()) {
                      result.parts.emplace_back(it2->second);
                    }
                  }
                },
                [&](const QString &text) { result.parts.emplace_back(ResultPart(text, false)); }};
    }

    return result;
  }

private:
  static constexpr int SHELL_TIMEOUT_MS = 2000;

  // FIXME: maybe we should execute this in a fully asynchronous manner, meaning the expansion itself
  // should be asynchronous. For most use cases it's fine to block, even if it timeouts it will only lock down
  // the UI for a brief moment (2s by default).
  static std::vector<QString> executeShellPlaceholdersSync(const PlaceholderString &parsed,
                                                           int timeout = SHELL_TIMEOUT_MS) {
    struct ShellCommand {
      QString code;
      QString exec;
    };

    std::vector<ShellCommand> commands;

    commands.reserve(4);

    for (const auto &part : parsed.parts()) {
      if (const auto *ph = std::get_if<PlaceholderString::ParsedPlaceholder>(&part)) {
        if (ph->id != "shell") continue;
        ShellCommand cmd;
        if (auto it = ph->args.find("code"); it != ph->args.end()) cmd.code = it->second;
        if (auto it = ph->args.find("exec"); it != ph->args.end())
          cmd.exec = it->second;
        else
          cmd.exec = qEnvironmentVariable("SHELL", QStringLiteral("/bin/sh"));
        commands.push_back(std::move(cmd));
      }
    }

    if (commands.empty()) return {};

    const auto runCommand = [timeout](const ShellCommand &cmd) -> QString {
      QProcess proc;
      proc.start(cmd.exec, {QStringLiteral("-c"), cmd.code});
      if (proc.waitForFinished(timeout) && proc.exitStatus() == QProcess::NormalExit &&
          proc.exitCode() == 0) {
        auto output = QString::fromUtf8(proc.readAllStandardOutput());
        if (output.endsWith('\n')) output.chop(1);
        return output;
      }
      qWarning() << "Shell placeholder failed:" << cmd.code << proc.errorString();
      return {};
    };

    std::vector<QFuture<QString>> futures;
    futures.reserve(commands.size());
    for (const auto &cmd : commands) {
      futures.push_back(QtConcurrent::run(runCommand, cmd));
    }

    std::vector<QString> results;
    results.reserve(futures.size());
    for (auto &future : futures) {
      future.waitForFinished();
      results.push_back(future.result());
    }

    return results;
  }

  QString m_uuid; // we generate it once to keep it stable across expansions, useful when presenting in list
                  // detail
};
