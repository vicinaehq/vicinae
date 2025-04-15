#include "ui/detail-widget.hpp"
#include "common.hpp"
#include "extend/metadata-model.hpp"
#include <qwidget.h>

QWidget *DetailWidget::contentWidget() const { return m_contentWidget; }

void DetailWidget::setContentWidget(QWidget *widget) {
  m_layout->replaceWidget(m_contentWidget, widget);
  m_contentWidget = widget;
}

void DetailWidget::clearMetadata() {
  m_layout->itemAt(1)->widget()->hide();
  m_metadata->hide();
}

void DetailWidget::setMetadata(const MetadataModel &model) {
  m_layout->itemAt(1)->widget()->show();

  for (const auto &item : model.children)
    m_metadata->addItem(item);

  m_metadata->show();
}

DetailWidget::DetailWidget(QWidget *parent) : QWidget(parent) {
  m_layout->addWidget(m_contentWidget, 1);
  m_layout->addWidget(new HDivider);
  m_layout->addWidget(m_metadata);
  m_layout->setContentsMargins(0, 0, 0, 0);
  m_layout->setSpacing(0);
  m_contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_metadata->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  clearMetadata();
  setLayout(m_layout);
}
