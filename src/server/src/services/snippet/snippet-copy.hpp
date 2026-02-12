#pragma once
#include "common/types.hpp"
#include "services/snippet/snippet-db.hpp"
#include "services/snippet/snippet-expander.hpp"
#include <qclipboard.h>
#include <qmimedata.h>
#include <ranges>

class SnippetCopy {
public:
  static void copyToClipboard(const snippet::SerializedSnippet &snippet,
                              const std::vector<std::pair<QString, QString>> args, QClipboard *clipboard) {
    auto mimeData = new QMimeData;

    mimeData->setData("vicinae/concealed", "1");

    const auto visitor =
        overloads{[&](const snippet::TextSnippet &text) {
                    SnippetExpander expander;
                    auto expanded = expander.expand(text.text.c_str(), args);
                    QString expandedText = expanded.parts |
                                           std::views::transform([](auto &&part) { return part.text; }) |
                                           std::views::join | std::ranges::to<QString>();

                    mimeData->setData("text/plain;charset=utf-8", expandedText.toUtf8());
                  },
                  [&](const snippet::FileSnippet &file) {
                    mimeData->setData("text/uri-list", QString("file://%1").arg(file.file).toUtf8());
                  }};

    std::visit(visitor, snippet.data);
    clipboard->setMimeData(mimeData);
  }
};
