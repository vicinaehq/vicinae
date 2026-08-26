#pragma once
#include "common/types.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/snippet/snippet-db.hpp"
#include "services/snippet/snippet-expander.hpp"
#include <ranges>

class SnippetCopy {
public:
  static bool copyToClipboard(const snippet::SerializedSnippet &snippet,
                              const std::vector<std::pair<QString, QString>> &args,
                              ClipboardService &clipman) {
    const auto visitor = overloads{
        [&](const snippet::TextSnippet &text) {
          SnippetExpander expander;
          auto expanded = expander.expand(text.text.c_str(), args);
          QString expandedText = expanded.parts |
                                 std::views::transform([](auto &&part) { return part.text; }) |
                                 std::views::join | std::ranges::to<QString>();

          return clipman.copyText(expandedText, {.transient = true});
        },
        [&](const snippet::FileSnippet &file) { return clipman.copyFile(file.file, {.transient = true}); }};

    return std::visit(visitor, snippet.data);
  }
};
