#include "split-detail.hpp"
#include <qtimer.h>
#include <qwidget.h>

void SplitDetailWidget::relayout() {
  auto currentSize = size();

  if (currentSize.isNull()) return;

  int ratioWidth = currentSize.width() * m_ratio;
  int mainWidth = currentSize.width() - ratioWidth;

  if (m_mainWidget) {
    QRect geo{0, 0, m_detailVisible ? mainWidth : currentSize.width(), currentSize.height()};
    m_mainWidget->setGeometry(geo);
    m_mainWidget->show();
  }
  if (m_detailWidget) { m_detailWidget->setGeometry({mainWidth, 0, ratioWidth, currentSize.height()}); }

  m_divider->setGeometry({mainWidth, 0, 1, currentSize.height()});

  if (m_detailWidget) { m_detailWidget->setVisible(m_detailVisible); }

  m_divider->setVisible(m_detailVisible);
}

void SplitDetailWidget::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  relayout();
}

QWidget *SplitDetailWidget::mainWidget() const { return m_mainWidget; }

QWidget *SplitDetailWidget::detailWidget() const { return m_detailWidget; }

void SplitDetailWidget::setMainWidget(QWidget *widget) {
  if (m_mainWidget) {
    // m_mainWidget->setParent(nullptr);
    m_mainWidget->hide();
  }

  widget->setParent(this);
  m_mainWidget = widget;
}

void SplitDetailWidget::setDetailWidget(QWidget *widget) {
  if (m_detailWidget) { m_detailWidget->deleteLater(); }

  widget->setParent(this);
  m_detailWidget = widget;
}

void SplitDetailWidget::setRatio(double ratio) {
  m_ratio = ratio;
  relayout();
}

void SplitDetailWidget::setDetailVisibility(bool value) {
  m_detailVisible = value;
  relayout();
}

double SplitDetailWidget::ratio() const { return m_ratio; }

bool SplitDetailWidget::isDetailVisible() const { return m_detailVisible; }

SplitDetailWidget::SplitDetailWidget(QWidget *parent) : QWidget(parent) {}
