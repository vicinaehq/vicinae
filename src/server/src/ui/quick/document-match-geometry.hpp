#pragma once
#include <QAbstractTextDocumentLayout>
#include <QRectF>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextLayout>
#include <algorithm>
#include <cmath>
#include <vector>

namespace document_match {

/// The area covered by a range of the document, one rectangle per line it spans.
inline std::vector<QRectF> rects(QTextDocument &document, int start, int length) {
  std::vector<QRectF> result;
  const int end = start + length;
  auto *layout = document.documentLayout();
  for (auto block = document.findBlock(start); block.isValid() && block.position() < end;
       block = block.next()) {
    const auto origin = layout->blockBoundingRect(block).topLeft();
    const auto *text = block.layout();
    for (int index = 0; index < text->lineCount(); ++index) {
      const auto line = text->lineAt(index);
      const int from = std::max(start - block.position(), line.textStart());
      const int to = std::min(end - block.position(), line.textStart() + line.textLength());
      if (from >= to) continue;
      const qreal left = line.cursorToX(from);
      const qreal right = line.cursorToX(to);
      result.emplace_back(origin.x() + std::min(left, right), origin.y() + line.y(), std::abs(right - left),
                          line.height());
    }
  }
  return result;
}

} // namespace document_match
