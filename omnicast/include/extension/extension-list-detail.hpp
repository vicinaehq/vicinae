#pragma once
#include "extend/detail-model.hpp"
#include "ui/detail-widget.hpp"
#include "ui/markdown/markdown-renderer.hpp"
#include <qevent.h>

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
};
