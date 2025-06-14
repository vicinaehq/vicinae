#include "ui/status_bar.hpp"
#include "common.hpp"
#include <qevent.h>
#include <qlogging.h>
#include <QPainterPath>
#include "omni-icon.hpp"
#include "services/toast/toast-service.hpp"
#include "theme.hpp"
#include "ui/shortcut-button.hpp"
#include "ui/toast.hpp"
#include "ui/typography.hpp"
#include <qnamespace.h>
#include <qpainter.h>
#include <qtimer.h>

StatusBar::StatusBar(QWidget *parent) : QWidget(parent), leftWidget(nullptr) {
  setAttribute(Qt::WA_TranslucentBackground);
  auto layout = new QHBoxLayout();

  setFixedHeight(40);

  leftSideWidget->addWidget(m_navigation);
  leftSideWidget->addWidget(m_toastWidget);
  leftSideWidget->setCurrentIndex(0);

  layout->setContentsMargins(15, 5, 15, 5);

  setProperty("class", "status-bar");

  leftWidget = new CurrentCommandWidget("", BuiltinOmniIconUrl("omnicast"));
  qDebug() << "set default navigation";

  right = new QWidget();
  auto rightLayout = new QHBoxLayout();

  layout->setAlignment(Qt::AlignVCenter);

  right->setLayout(rightLayout);
  rightLayout->setContentsMargins(0, 0, 0, 0);

  selectedActionLabel = new TypographyWidget();
  _selectedActionButton = new ShortcutButton();

  _selectedActionButton->setMaximumWidth(200);

  rightLayout->addWidget(_selectedActionButton);
  _rightDivider = new VDivider();
  _rightDivider->setContentsMargins(0, 5, 0, 5);
  _rightDivider->setWidth(2);
  rightLayout->addWidget(_rightDivider);

  _actionButton = new ShortcutButton();
  _actionButton->setTextColor("#AAAAAA");
  _actionButton->setText("Actions");
  _actionButton->setShortcut(KeyboardShortcutModel{.key = "B", .modifiers = {"ctrl"}});
  _actionButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this](const ThemeInfo &info) {
    _selectedActionButton->resetColor();
    _actionButton->resetColor();
  });

  _selectedActionButton->resetColor();
  _actionButton->resetColor();

  rightLayout->addWidget(_actionButton, 0, Qt::AlignRight);

  layout->addWidget(leftSideWidget, 0, Qt::AlignLeft);
  layout->addWidget(right, 0, Qt::AlignRight);

  setLayout(layout);

  connect(_actionButton, &ShortcutButton::clicked, this, &StatusBar::actionButtonClicked);
  connect(_selectedActionButton, &ShortcutButton::clicked, this, &StatusBar::currentActionButtonClicked);
}

void StatusBar::setCurrentAction(const QString &action, const KeyboardShortcutModel &shortcut) {
  _selectedActionButton->setText(action);
  _selectedActionButton->setShortcut(shortcut);
  right->show();
}

void StatusBar::setActionButtonVisibility(bool value) {
  right->setVisible(value);
  _actionButton->setVisible(value);
  _rightDivider->setVisible(value && _selectedActionButton->isVisible());
}

void StatusBar::setAction(const AbstractAction &action) {
  _selectedActionButton->setText(action.title());
  if (action.shortcut) { _selectedActionButton->setShortcut(*action.shortcut); }
  right->show();
}

void StatusBar::setCurrentActionButtonVisibility(bool value) {
  _selectedActionButton->setVisible(value);
  _rightDivider->setVisible(value && _actionButton->isVisible());
}

KeyboardShortcutModel StatusBar::actionButtonShortcut() const { return _actionButton->shortcut(); }

void StatusBar::setActionButton(const QString &title, const KeyboardShortcutModel &shortcut) {
  _actionButton->setText(title);
  _actionButton->setShortcut(shortcut);
}

void StatusBar::setActionButtonHighlight(bool highlight) { _actionButton->hoverChanged(highlight); }

void StatusBar::clearAction() { right->hide(); }

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
  setNavigationTitle(name);
  setNavigationIcon(icon);
}

void StatusBar::setNavigationTitle(const QString &title) { m_navigation->setTitle(title); }

QString StatusBar::navigationTitle() const { return m_navigation->title(); }

OmniIconUrl StatusBar::navigationIcon() const { return m_navigation->icon(); }

void StatusBar::setNavigationIcon(const OmniIconUrl &icon) { m_navigation->setIcon(icon); }

bool StatusBar::isActionButtonVisible() const { return _actionButton->isVisible(); }

void StatusBar::reset() { setLeftWidget(new DefaultLeftWidget()); }

void StatusBar::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  QPainter painter(this);
  int radius = 10;
  int borderWidth = 1;
  QPen pen(theme.colors.statusBackgroundBorder, borderWidth);

  painter.setRenderHint(QPainter::Antialiasing, true);

  painter.setBrush(theme.colors.statusBackground);
  painter.setPen(QPen(theme.colors.border));
  painter.drawRoundedRect(rect(), radius, radius);
  painter.setPen(Qt::NoPen);
  // fill top to get rid of the top border
  painter.drawRect(borderWidth, 0, width() - borderWidth * 2, height() - radius);

  painter.setBrush(theme.colors.border);
  painter.drawRect(0, 0, width(), 1);
}

void StatusBar::clearToast() { leftSideWidget->setCurrentIndex(0); }

void StatusBar::setToast(Toast const *toast) {
  m_toast = toast;
  m_toastWidget->setToast(toast);
  leftSideWidget->setCurrentIndex(1);

  connect(toast, &Toast::updated, this, [this, toast]() { m_toastWidget->setToast(toast); });
}
