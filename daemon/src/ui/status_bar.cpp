#include "ui/status_bar.hpp"
#include "common.hpp"
#include "extend/image-model.hpp"
#include <qevent.h>
#include <qlogging.h>
#include <QPainterPath>
#include "ui/shortcut-button.hpp"
#include <qnamespace.h>
#include <qpainter.h>
#include <qtimer.h>

StatusBar::StatusBar(QWidget *parent) : QWidget(parent), leftWidget(nullptr) {
  auto layout = new QHBoxLayout();

  setFixedHeight(40);

  layout->setContentsMargins(15, 5, 15, 5);

  setProperty("class", "status-bar");

  leftWidget = new DefaultLeftWidget();

  auto right = new QWidget();
  auto rightLayout = new QHBoxLayout();

  layout->setAlignment(Qt::AlignVCenter);

  right->setLayout(rightLayout);
  rightLayout->setContentsMargins(0, 0, 0, 0);

  selectedActionLabel = new QLabel();
  _selectedActionButton = new ShortcutButton();

  _selectedActionButton->setMaximumWidth(200);
  _selectedActionButton->hide();

  rightLayout->addWidget(_selectedActionButton);
  auto divider = new VDivider();

  divider->setContentsMargins(0, 5, 0, 5);
  divider->setWidth(2);

  rightLayout->addWidget(divider);

  _actionButton = new ShortcutButton();
  _actionButton->setTextColor("#AAAAAA");
  _actionButton->setText("Actions");
  _actionButton->setShortcut(KeyboardShortcutModel{.key = "B", .modifiers = {"ctrl"}});
  _actionButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  rightLayout->addWidget(_actionButton, 0, Qt::AlignRight);

  layout->addWidget(leftWidget, 0, Qt::AlignLeft);
  layout->addWidget(right, 0, Qt::AlignRight);

  setLayout(layout);

  connect(_actionButton, &ShortcutButton::clicked, this, &StatusBar::actionButtonClicked);
  connect(_selectedActionButton, &ShortcutButton::clicked, this, &StatusBar::currentActionButtonClicked);
}

void StatusBar::setAction(const AbstractAction &action) {
  _selectedActionButton->setText(action.title);
  if (action.shortcut) { _selectedActionButton->setShortcut(*action.shortcut); }
  _selectedActionButton->show();
}

void StatusBar::setActionButtonHighlight(bool highlight) { _actionButton->setHovered(highlight); }

void StatusBar::clearAction() { _selectedActionButton->hide(); }

void StatusBar::setLeftWidget(QWidget *left) {
  QWidget *oldWidget = leftWidget;

  layout()->replaceWidget(oldWidget, left);

  if (oldWidget && oldWidget) oldWidget->deleteLater();

  if (tmpLeft) {
    tmpLeft->deleteLater();
    tmpLeft = nullptr;
  }

  leftWidget = left;
}

void StatusBar::setNavigation(const QString &name, const ImageLikeModel &icon) {
  setLeftWidget(new CurrentCommandWidget(name, icon));
}

void StatusBar::setNavigationTitle(const QString &title) {
  if (auto left = dynamic_cast<CurrentCommandWidget *>(leftWidget)) { left->setTitle(title); }
}

QString StatusBar::navigationTitle() const {
  if (auto left = dynamic_cast<CurrentCommandWidget *>(leftWidget)) { return left->title(); }

  return "";
}

void StatusBar::reset() { setLeftWidget(new DefaultLeftWidget()); }

void StatusBar::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);

  // QColor backgroundColor("#121212");

  // painter.fillRect(this->rect().adjusted(1, 1, -1, -1), backgroundColor);
}

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
