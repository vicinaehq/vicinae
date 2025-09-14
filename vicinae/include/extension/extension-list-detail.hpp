#pragma once
#include "common.hpp"
#include "extend/detail-model.hpp"
#include "ui/horizontal-metadata.hpp"
#include "ui/markdown/markdown-renderer.hpp"
#include "utils/layout.hpp"
#include <qcoreevent.h>
#include <qevent.h>
#include <qlogging.h>
#include <qminmax.h>
#include <qnamespace.h>
#include <qobject.h>

class ExtensionListDetail : public QWidget {
  MarkdownRenderer *markdownRenderer = new MarkdownRenderer;
  HDivider *m_divider = new HDivider;
  HorizontalMetadata *m_metadata = new HorizontalMetadata;

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    recalculateMetadata();
  }

  void recalculateMetadata() {
    int maxMetadataHeight = markdownRenderer->isVisible() ? height() * 0.5 : height();
    int widgetHeightHint = m_metadata->widget()->sizeHint().height() + 10;

    m_metadata->setFixedHeight(qMin(maxMetadataHeight, widgetHeightHint));
  }

  void setupUI() {
    VStack().add(markdownRenderer, 1).add(m_divider).add(m_metadata).addStretch().imbue(this);
    m_metadata->hide();
    connect(m_metadata, &VerticalScrollArea::widgetResized, this, &ExtensionListDetail::recalculateMetadata);
  };

public:
  ExtensionListDetail() { setupUI(); }

  void setDetail(const DetailModel &model) {
    if (model.markdown) { markdownRenderer->setMarkdown(*model.markdown); }

    markdownRenderer->setVisible(model.markdown.has_value());

    m_metadata->setMetadata(model.metadata.children | std::ranges::to<std::vector>());
    bool hasMeta = !model.metadata.children.isEmpty();

    m_metadata->setVisible(hasMeta);
    m_divider->setVisible(hasMeta && model.markdown.has_value());
  }

  void updateDetail(const DetailModel &model) {
    if (model.markdown) {
      // optimization to append markdown, particularily useful when content is streamed
      if (model.markdown->startsWith(markdownRenderer->markdown())) {
        auto appended = QStringView(*model.markdown).sliced(markdownRenderer->markdown().size());

        if (!appended.isEmpty()) { markdownRenderer->appendMarkdown(appended); }
      } else {
        markdownRenderer->setMarkdown(*model.markdown);
      }
    }

    bool hasMeta = !model.metadata.children.isEmpty();

    m_metadata->setMetadata(model.metadata.children | std::ranges::to<std::vector>());
    m_metadata->setVisible(hasMeta);
    m_divider->setVisible(hasMeta && model.markdown.has_value());
  }
};
