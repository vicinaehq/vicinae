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
#include "utils/placeholder.hpp"
#include "services/app-service/app-service.hpp"
#include "services/selection/abstract-selection-service.hpp"
#include "service-registry.hpp"

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

  explicit SnippetExpander(const AppService *appService = nullptr)
      : m_appService(appService), m_uuid(QUuid::createUuid().toString(QUuid::WithoutBraces)) {}

  explicit SnippetExpander(const AppService &appService) : SnippetExpander(&appService) {}

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
    auto *clip = QGuiApplication::clipboard();
    const auto parsed = PlaceholderString::parseSnippetText(text);

    const auto shellResults =
        opts.executeShell ? executeShellPlaceholdersSync(parsed) : std::vector<QString>{};
    int shellIdx = 0;
    Result result;

    for (const auto &part : parsed.parts()) {
      match(
          part,
          [&](const PlaceholderString::ParsedPlaceholder &placeholder) {
            if (placeholder.id == "cursor") {
              result.cursorPos =
                  std::ranges::fold_left(result.parts, 0, [](int n, auto &&s) { return n + s.text.size(); });
              return;
            }

            QString val;

            if (placeholder.id == "clipboard") {
              if (clip) val = clip->text();
            } else if (placeholder.id == "selection" || placeholder.id == "selected") {
              if (auto *reg = ServiceRegistry::instance()) {
                if (auto selService = reg->selectionService()) {
                  if (auto selected = selService->selectedTextSync()) { val = *selected; }
                }
              }
            } else if (placeholder.id == "uuid") {
              val = m_uuid;
            } else if (placeholder.id == "date" || placeholder.id == "datetime" || placeholder.id == "time" ||
                       placeholder.id == "day") {
              QDateTime dt = QDateTime::currentDateTime();
              if (auto it = placeholder.args.find("offset"); it != placeholder.args.end()) {
                dt = placeholder_utils::applyDateOffsets(dt, it->second);
              }
              QString fmt;
              if (auto it = placeholder.args.find("format"); it != placeholder.args.end()) {
                fmt = placeholder_utils::normalizeDateFormat(it->second);
              } else if (placeholder.id == "time") {
                fmt = QStringLiteral("HH:mm");
              } else if (placeholder.id == "day") {
                fmt = QStringLiteral("dddd");
              } else if (placeholder.id == "datetime") {
                fmt = QStringLiteral("yyyy-MM-dd HH:mm:ss");
              } else {
                fmt = QStringLiteral("yyyy-MM-dd");
              }
              val = dt.toString(fmt);
            } else if (placeholder.id == "shell") {
              if (opts.executeShell && shellIdx < static_cast<int>(shellResults.size())) {
                val = shellResults[shellIdx];
              } else {
                auto code = placeholder.args.contains("code") ? placeholder.args.at("code") : QString();
                val = QStringLiteral("$(%1)").arg(code);
              }
              ++shellIdx;
            } else if (placeholder.id == "argument") {
              QString name;
              if (const auto it = placeholder.args.find("name"); it != placeholder.args.end()) {
                name = it->second;
              }
              if (const auto it2 =
                      std::ranges::find_if(arguments, [&](auto &&pair) { return pair.first == name; });
                  it2 != arguments.end() && !it2->second.isEmpty()) {
                val = it2->second;
              } else if (auto itDef = placeholder.args.find("default"); itDef != placeholder.args.end()) {
                val = itDef->second;
              }
            } else {
              if (const auto it2 = std::ranges::find_if(
                      arguments, [&](auto &&pair) { return pair.first == placeholder.id; });
                  it2 != arguments.end() && !it2->second.isEmpty()) {
                val = it2->second;
              } else if (auto itDef = placeholder.args.find("default"); itDef != placeholder.args.end()) {
                val = itDef->second;
              }
            }

            if (val.isEmpty() && placeholder.fallbackId.has_value()) {
              const auto &fb = *placeholder.fallbackId;
              if (fb == "clipboard") {
                if (clip) val = clip->text();
              } else if (fb == "selection" || fb == "selected") {
                if (auto *reg = ServiceRegistry::instance()) {
                  if (auto selService = reg->selectionService()) {
                    if (auto selected = selService->selectedTextSync()) { val = *selected; }
                  }
                }
              } else if (const auto it2 =
                             std::ranges::find_if(arguments, [&](auto &&pair) { return pair.first == fb; });
                         it2 != arguments.end() && !it2->second.isEmpty()) {
                val = it2->second;
              } else {
                val = fb;
              }
            }

            for (const auto &mod : placeholder.modifiers) {
              val = placeholder_utils::applyModifier(val, mod);
            }

            result.parts.emplace_back(ResultPart(val, true));
          },
          [&](const QString &text) { result.parts.emplace_back(ResultPart(text, false)); });
    }

    return result;
  }

private:
  static constexpr int SHELL_TIMEOUT_MS = 2000;

  // Shell placeholders run synchronously during expansion (or asynchronously when prefetching)
  std::vector<QString> executeShellPlaceholdersSync(const PlaceholderString &parsed,
                                                    int timeout = SHELL_TIMEOUT_MS) const {
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
        if (auto it = ph->args.find("exec"); it != ph->args.end()) cmd.exec = it->second;
        commands.push_back(std::move(cmd));
      }
    }

    if (commands.empty()) return {};

    const auto runCommand = [this, timeout](const ShellCommand &cmd) -> QString {
      std::unique_ptr<QProcess> proc;
      if (cmd.exec.isEmpty()) {
        if (m_appService) {
          proc = m_appService->shellProcess(cmd.code);
        } else {
          proc = std::make_unique<QProcess>();
          proc->setProgram(QStringLiteral("/bin/sh"));
          proc->setArguments({QStringLiteral("-c"), cmd.code});
        }
      } else {
        proc = std::make_unique<QProcess>();
        proc->setProgram(cmd.exec);
        proc->setArguments({QStringLiteral("-c"), cmd.code});
      }

      proc->start();
      if (proc->waitForFinished(timeout) && proc->exitStatus() == QProcess::NormalExit &&
          proc->exitCode() == 0) {
        return QString::fromUtf8(proc->readAllStandardOutput()).trimmed();
      }
      qWarning() << "Shell placeholder failed:" << cmd.code << proc->errorString();
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

  const AppService *m_appService = nullptr;
  QString m_uuid; // we generate it once to keep it stable across expansions
};
