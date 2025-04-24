#pragma once
#include "extend/detail-model.hpp"
#include "ui/detail-widget.hpp"
#include "ui/markdown/markdown-renderer.hpp"
#include <qevent.h>
#include <qnamespace.h>

class ExtensionListDetail : public DetailWidget {
  MarkdownRenderer *markdownRenderer = new MarkdownRenderer;

  void resizeEvent(QResizeEvent *event) override {
    DetailWidget::resizeEvent(event);
    qDebug() << "detail resize event" << event->size();
  }

public:
  ExtensionListDetail() { setContentWidget(markdownRenderer); }

  void setDetail(const DetailModel &model) {
    markdownRenderer->setMarkdown(model.markdown);
    if (!model.metadata.children.isEmpty()) { setMetadata(model.metadata); }
  }

  void updateDetail(const DetailModel &model) {
    // optimization to append markdown, particularily useful when content is streamed
    if (model.markdown.startsWith(markdownRenderer->markdown())) {
      auto appended = QStringView(model.markdown).sliced(markdownRenderer->markdown().size());

      if (!appended.isEmpty()) { markdownRenderer->appendMarkdown(appended); }
    } else {
      markdownRenderer->setMarkdown(model.markdown);
    }

    if (!model.metadata.children.isEmpty()) { setMetadata(model.metadata); }
  }
};
