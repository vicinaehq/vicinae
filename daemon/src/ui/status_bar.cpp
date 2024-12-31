#include "ui/status_bar.hpp"
#include <qlogging.h>
#include <qnamespace.h>
#include <qtimer.h>

StatusBar::StatusBar(QWidget *parent) : QWidget(parent), leftWidget(nullptr) {
  auto layout = new QHBoxLayout();

  setFixedHeight(40);

  layout->setContentsMargins(15, 0, 15, 0);

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

  if (oldWidget && oldWidget)
    oldWidget->deleteLater();

  if (tmpLeft) {
    tmpLeft->deleteLater();
    tmpLeft = nullptr;
  }

  leftWidget = left;
}

void StatusBar::setActiveCommand(const QString &name, QIcon icon) {
  setLeftWidget(new CurrentCommandWidget(name, icon));
}

void StatusBar::reset() { setLeftWidget(new DefaultLeftWidget()); }

void StatusBar::setToast(const QString &text, ToastPriority priority) {
  auto toast = new ToastWidget(text, priority);

  connect(toast, &ToastWidget::fadeOut, [this]() {
    auto old = tmpLeft;

    tmpLeft = nullptr;
    old->setParent(this);
    old->show();
    setLeftWidget(old);
  });

  layout()->replaceWidget(leftWidget, toast);

  if (!tmpLeft) {
    tmpLeft = leftWidget;
    tmpLeft->setParent(nullptr);
    tmpLeft->hide();
  } else {
    // delete previous toast
    leftWidget->deleteLater();
  }

  leftWidget = toast;
  toast->start(2000);
}
