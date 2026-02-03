#include "simple-view.hpp"
#include <qboxlayout.h>

void SimpleView::setupUI(QWidget *centerWidget) {
  QVBoxLayout *m_layout = new QVBoxLayout;
  m_layout->setContentsMargins(0, 0, 0, 0);
  m_layout->setSpacing(0);
  // m_layout->addWidget(m_topBar);
  m_layout->addWidget(centerWidget, 1);
  setLayout(m_layout);
}

QWidget *SimpleView::centerWidget() const {
  return new QWidget;
}

SimpleView::SimpleView(QWidget *parent) : BaseView(parent) {
  // m_topBar->showBackButton();
}
