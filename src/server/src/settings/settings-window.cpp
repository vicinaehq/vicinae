#include <qevent.h>
#include <qnamespace.h>
#include <qwidget.h>
#include "settings-window.hpp"
#include "service-registry.hpp"
#include "settings-controller/settings-controller.hpp"
#include "services/background-effect/background-effect-manager.hpp"
#include "theme.hpp"
#include "vicinae.hpp"

static constexpr QSize windowSize(1000, 600);

void SettingsWindow::paintEvent(QPaintEvent *event) {
  auto &config = ServiceRegistry::instance()->config()->value();
  auto &theme = ThemeService::instance().theme();
  OmniPainter painter(this);
  QColor finalBgColor = painter.resolveColor(SemanticColor::Background);

  finalBgColor.setAlphaF(config.launcherWindow.opacity);
  painter.setRenderHint(QPainter::Antialiasing);
  QPainterPath path;
  path.addRoundedRect(rect(), 10, 10);
  painter.setClipPath(path);
  painter.fillPath(path, finalBgColor);
  painter.setThemePen(SemanticColor::BackgroundBorder, Omnicast::WINDOW_BORDER_WIDTH);
  painter.drawPath(path);
}

void SettingsWindow::keyPressEvent(QKeyEvent *event) {
  if (event->modifiers().toInt() == 0 && event->key() == Qt::Key_Escape) {
    close();
    return;
  }

  QWidget::keyPressEvent(event);
}

QWidget *SettingsWindow::createWidget() {
  QWidget *widget = new QWidget;
  QVBoxLayout *layout = new QVBoxLayout;

  for (const auto &category : m_categories) {
    m_navigation->addPane(category->id(), category->title(), category->icon());
    content->addWidget(category->createContent(m_ctx));
  }

  connect(m_navigation, &SettingsNavWidget::rowChanged, this, [this](int idx) {
    content->setCurrentIndex(idx);
    m_navigation->setSelected(m_categories.at(idx)->id());
  });

  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(m_navigation);
  layout->addWidget(new HDivider);
  layout->addWidget(content, 1);
  widget->setLayout(layout);

  return widget;
}

void SettingsWindow::resizeEvent(QResizeEvent *event) {
  QMainWindow::resizeEvent(event);

  auto &cfg = m_ctx->services->config()->value();
  auto bgEffectManager = m_ctx->services->backgroundEffectManager();

  if (bgEffectManager->supportsBlur()) {
    if (cfg.launcherWindow.blur.enabled) {
      bgEffectManager->setBlur(windowHandle(), {.radius = 10, .region = rect()});
    } else {
      bgEffectManager->clearBlur(windowHandle());
    }
  }
}

void SettingsWindow::showEvent(QShowEvent *event) {
  QMainWindow::showEvent(event);

  auto &cfg = m_ctx->services->config()->value();
  auto bgEffectManager = m_ctx->services->backgroundEffectManager();

  if (bgEffectManager->supportsBlur()) {
    if (cfg.launcherWindow.blur.enabled) {
      bgEffectManager->setBlur(windowHandle(), {.radius = 10, .region = rect()});
    } else {
      bgEffectManager->clearBlur(windowHandle());
    }
  }
}

void SettingsWindow::hideEvent(QHideEvent *event) {
  QWidget::hideEvent(event);
  m_ctx->settings->closeWindow();
};

SettingsWindow::SettingsWindow(ApplicationContext *ctx) : m_ctx(ctx) {
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
  setMinimumSize(windowSize);
  setMaximumSize(windowSize);
  setWindowTitle("Vicinae Settings");
  m_categories.reserve(4);
  m_categories.emplace_back(std::make_unique<GeneralSettingsCategory>());
  m_categories.emplace_back(std::make_unique<ExtensionSettingsCategory>());
  m_categories.emplace_back(std::make_unique<KeybindSettingsCategory>());
  m_categories.emplace_back(std::make_unique<AboutSettingsCategory>());
  setCentralWidget(createWidget());

  connect(m_ctx->settings.get(), &SettingsController::windowVisiblityChangeRequested, this,
          [this](bool value) {
            hide();
            setVisible(value);
          });

  connect(m_ctx->settings.get(), &SettingsController::tabIdOpened, this, [this](const QString &id) {
    if (auto it = std::ranges::find_if(m_categories, [&](auto &&cat) { return cat->id() == id; });
        it != m_categories.end()) {
      m_navigation->setSelected(id);
      content->setCurrentIndex(std::distance(m_categories.begin(), it));
    }
  });

  m_navigation->setSelected("general");
  content->setCurrentIndex(0);
}
