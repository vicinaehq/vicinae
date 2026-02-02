#include "detail-widget.hpp"
#include "ui/scroll-bar/scroll-bar.hpp"

void DetailWidget::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  m_metadata->setMaximumHeight(height() * 0.4);
}

void DetailWidget::setMetadata(const std::vector<MetadataItem> &items) {
  m_metadata->setMetadata(items);
  hdivider->show();
  m_metadata->show();
}

QWidget *DetailWidget::content() const { return m_contentScrollArea->widget(); }

void DetailWidget::clearMetadata() {
  // m_metadata->clear();
  hdivider->hide();
  m_metadata->hide();
}

void DetailWidget::setContent(QWidget *widget) {
  m_contentScrollArea->setWidget(widget);
  m_contentScrollArea->setWidgetResizable(true);
  m_contentScrollArea->setAutoFillBackground(false);
  setAutoFillBackground(false);
}

void DetailWidget::setupUI() {
  auto layout = new QVBoxLayout;

  m_contentScrollArea->setVerticalScrollBar(new OmniScrollBar);
  m_contentScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_contentScrollArea->setWidgetResizable(true);
  m_contentScrollArea->setAutoFillBackground(false);
  setAutoFillBackground(false);
  m_contentScrollArea->setAttribute(Qt::WA_TranslucentBackground);

  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_contentScrollArea, 1);
  m_contentScrollArea->setFocusPolicy(Qt::NoFocus);
  layout->addWidget(hdivider);
  layout->addWidget(m_metadata);
  setLayout(layout);

  hdivider->hide();
  m_metadata->hide();
}
