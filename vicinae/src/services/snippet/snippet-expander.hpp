#pragma once
#include <optional>
#include <qapplication.h>
#include <qclipboard.h>
#include <qcontainerfwd.h>
#include <quuid.h>
#include <ranges>
#include "common/types.hpp"
#include "placeholder.hpp"

class SnippetExpander {
  struct ResultPart {
    QString text;
    bool placeholder = false;
  };

  struct Result {
    std::vector<ResultPart> parts;
    std::optional<int> cursorPos;
  };

public:
  SnippetExpander() : m_uuid(QUuid::createUuid().toString(QUuid::WithoutBraces)) {}

  QString expandToString(const QString &text,
                         const std::vector<std::pair<QString, QString>> &arguments) const {
    const auto expanded = expand(text, arguments);

    return expanded.parts | std::views::transform([](auto &&pair) { return pair.text; }) | std::views::join |
           std::ranges::to<QString>();
  }

  Result expand(const QString &text, const std::vector<std::pair<QString, QString>> &arguments) const {
    const auto clip = QApplication::clipboard();
    const auto parsed = PlaceholderString::parseSnippetText(text);
    Result result;

    std::ranges::for_each(parsed.parts(), [&](auto &part) {
      part | match{[&](const PlaceholderString::ParsedPlaceholder placeholder) {
                     if (placeholder.id == "cursor") {
                       result.cursorPos = std::ranges::fold_left(
                           result.parts, 0, [](int n, auto &&s) { return n + s.text.size(); });
                     } else if (placeholder.id == "clipboard") {
                       result.parts.emplace_back(clip->text(), true);
                     } else if (placeholder.id == "uuid") {
                       result.parts.emplace_back(ResultPart(m_uuid, true));
                     } else if (placeholder.id == "date") {
                       result.parts.emplace_back(ResultPart("now", true));
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
    });

    return result;
  }

private:
  QString m_uuid; // we generate it once to keep it stable across expansions, useful when presenting in list
                  // detail
};
