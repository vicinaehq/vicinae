#pragma once
#include "common/types.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/snippet/snippet-db.hpp"
#include "services/snippet/snippet-expander.hpp"

class SnippetCopy {
public:
  static Clipboard::Content content(const snippet::SerializedSnippet &snippet,
                                    const std::vector<std::pair<QString, QString>> &args) {
    return std::visit(overloads{[&](const snippet::TextSnippet &text) -> Clipboard::Content {
                                  SnippetExpander expander;
                                  return Clipboard::Text{expander.expandToString(text.text.c_str(), args)};
                                },
                                [&](const snippet::FileSnippet &file) -> Clipboard::Content {
                                  return Clipboard::File{file.file};
                                }},
                      snippet.data);
  }

  static bool copyToClipboard(const snippet::SerializedSnippet &snippet,
                              const std::vector<std::pair<QString, QString>> &args,
                              ClipboardService &clipman) {
    return clipman.copyContent(content(snippet, args), {.transient = true});
  }
};
