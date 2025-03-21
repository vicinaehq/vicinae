#include "ui/status_bar.hpp"
#include "common.hpp"
#include <qevent.h>
#include <qlogging.h>
#include <QPainterPath>
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/shortcut-button.hpp"
#include <qnamespace.h>
#include <qpainter.h>
#include <qtimer.h>

StatusBar::StatusBar(QWidget *parent) : QWidget(parent), leftWidget(nullptr) {
  setAttribute(Qt::WA_TranslucentBackground);
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

  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this](const ThemeInfo &info) {
    _selectedActionButton->resetColor();
    _actionButton->resetColor();
  });

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

void StatusBar::setActionButtonHighlight(bool highlight) { _actionButton->hoverChanged(highlight); }

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

void StatusBar::setNavigation(const QString &name, const OmniIconUrl &icon) {
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
  auto &theme = ThemeService::instance().theme();
  QPainter painter(this);
  int radius = 10;
  int borderWidth = 1;
  QPen pen(theme.colors.statusBackgroundBorder, borderWidth);

  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setBrush(theme.colors.statusBackground);
  painter.setPen(pen);
  painter.drawRoundedRect(rect(), radius, radius);
  painter.setPen(Qt::NoPen);
  // fill top to get rid of the top border
  painter.drawRect(borderWidth, 0, width() - borderWidth * 2, height() - radius);
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
