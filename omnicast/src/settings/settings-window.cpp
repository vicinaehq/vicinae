#include <qevent.h>
#include <qwidget.h>
#include "settings-window.hpp"
#include "common.hpp"
#include "service-registry.hpp"

static constexpr QSize windowSize(1000, 600);

void SettingsWindow::paintEvent(QPaintEvent *event) {
  auto &config = ServiceRegistry::instance()->config()->value();
  auto &theme = ThemeService::instance().theme();
  int borderWidth = 1;
  QColor finalBgColor = theme.colors.mainBackground;
  QPainter painter(this);

  finalBgColor.setAlphaF(config.window.opacity);

  painter.setRenderHint(QPainter::Antialiasing, true);

  QPainterPath path;
  path.addRoundedRect(rect(), config.window.rounding, config.window.rounding);

  painter.setClipPath(path);

  painter.fillPath(path, finalBgColor);

  QPen pen(theme.colors.border, borderWidth);
  painter.setPen(pen);

  painter.drawPath(path);
}

QWidget *SettingsWindow::createWidget() {
  QWidget *widget = new QWidget;
  QVBoxLayout *layout = new QVBoxLayout;

  for (const auto &category : m_categories) {
    m_navigation->addPane(category->title(), category->icon());
    content->addWidget(category->createContent());
  }

  connect(m_navigation, &SettingsNavWidget::rowChanged, this, [this](int idx) {
    content->setCurrentIndex(idx);
    m_navigation->setSelected(m_categories.at(idx)->title());
  });

  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(m_navigation);
  layout->addWidget(new HDivider);
  layout->addWidget(content, 1);
  widget->setLayout(layout);

  return widget;
}

void SettingsWindow::showEvent(QShowEvent *event) {
  m_navigation->setSelected("Extensions");
  content->setCurrentIndex(1);
  QMainWindow::showEvent(event);
}

SettingsWindow::SettingsWindow() {
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground, true);
  setMinimumSize(windowSize);

  m_categories.reserve(4);
  m_categories.emplace_back(std::make_unique<GeneralSettingsCategory>());
  m_categories.emplace_back(std::make_unique<ExtensionSettingsCategory>());
  m_categories.emplace_back(std::make_unique<AdvancedSettingsCategory>());
  m_categories.emplace_back(std::make_unique<AboutSettingsCategory>());
  setCentralWidget(createWidget());
}

SettingsWindow::~SettingsWindow() {}
