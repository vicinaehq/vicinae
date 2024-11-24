#include "ui/status_bar.hpp"
#include <qnamespace.h>

void StatusBar::setSelectedAction(const std::shared_ptr<IAction> &action) {
  selectedActionLabel->setText(action->name());
}

StatusBar::StatusBar(QWidget *parent) : QWidget(parent), leftWidget(nullptr) {
  auto layout = new QHBoxLayout();

  layout->setContentsMargins(15, 8, 15, 8);

  setProperty("class", "status-bar");

  leftWidget = new DefaultLeftWidget();

  auto right = new QWidget();
  auto rightLayout = new QHBoxLayout();

  right->setLayout(rightLayout);
  rightLayout->setContentsMargins(0, 0, 0, 0);

  selectedActionLabel = new QLabel();

  rightLayout->addWidget(selectedActionLabel);
  rightLayout->addWidget(new QLabel("Actions"));

  layout->addWidget(leftWidget, 0, Qt::AlignLeft);
  layout->addWidget(right, 0, Qt::AlignRight);

  setLayout(layout);
}

void StatusBar::setLeftWidget(QWidget *left) {
  QWidget *oldWidget = leftWidget;

  layout()->replaceWidget(oldWidget, left);

  if (oldWidget)
    oldWidget->deleteLater();

  leftWidget = left;
}

void StatusBar::setActiveCommand(const QString &name, const QString &icon) {
  setLeftWidget(new CurrentCommandWidget(name, icon));
}

void StatusBar::reset() { setLeftWidget(new DefaultLeftWidget()); }
