#pragma once
#include "common.hpp"
#include "../ui/image/url.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include "ui/image/image.hpp"
#include <qevent.h>
#include <ranges>
#include "settings-category.hpp"
#include "ui/typography/typography.hpp"
#include "ui/vertical-scroll-area/vertical-scroll-area.hpp"
#include <qboxlayout.h>
#include <qlocale.h>
#include <qmainwindow.h>
#include <qnamespace.h>
#include <qpainterpath.h>
#include <qstackedwidget.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class SettingsNavPane : public QWidget {
  Q_OBJECT

  ImageWidget *m_icon = new ImageWidget;
  TypographyWidget *m_title = new TypographyWidget;
  bool m_hovered = false;
  bool m_selected = false;

  void mousePressEvent(QMouseEvent *event) override {
    QWidget::mousePressEvent(event);
    emit clicked();
  }

  void setForeground(const ColorLike &color) {
    ImageURL url = m_icon->url();

    url.setFill(color);
    m_title->setColor(color);
    m_icon->setUrl(url);
  }

  bool event(QEvent *event) override {
    switch (event->type()) {
    case QEvent::HoverEnter: {
      setForeground(SemanticColor::Foreground);
      m_hovered = true;
      break;
    }
    case QEvent::HoverLeave: {
      setForeground(m_selected ? SemanticColor::Foreground : SemanticColor::TextMuted);
      m_hovered = false;
      break;
    }
    default:
      break;
    }

    return QWidget::event(event);
  }

  QColor backgroundColor() {
    auto &theme = ThemeService::instance().theme();
    OmniPainter painter;

    if (m_selected) { return painter.resolveColor(SemanticColor::ListItemSelectionBackground); }
    if (m_hovered) { return painter.resolveColor(SemanticColor::ListItemHoverBackground); }

    return painter.resolveColor(SemanticColor::Background);
  }

  void paintEvent(QPaintEvent *event) override {
    auto &theme = ThemeService::instance().theme();
    int borderWidth = 1;
    QColor finalBgColor = backgroundColor();
    QPainter painter(this);
    QPainterPath path;

    if (m_hovered || m_selected) {
      painter.setRenderHint(QPainter::Antialiasing, true);
      path.addRoundedRect(rect(), 6, 6);
      painter.setClipPath(path);
      painter.fillPath(path, finalBgColor);
      painter.setPen(Qt::NoPen);
      painter.drawPath(path);
    }
  }

public:
  void select() {
    m_selected = true;
    setForeground(SemanticColor::Foreground);
    update();
  }

  void deselect() {
    m_selected = false;
    setForeground(SemanticColor::TextMuted);
    update();
  }

  void setupUI() {
    QVBoxLayout *layout = new QVBoxLayout;

    setAttribute(Qt::WA_Hover);
    m_title->setAlignment(Qt::AlignCenter);
    m_title->setColor(SemanticColor::TextMuted);
    m_icon->setFixedSize(20, 20);
    layout->setContentsMargins(0, 5, 0, 5);
    layout->addWidget(m_icon, 0, Qt::AlignCenter);
    layout->addWidget(m_title);

    setLayout(layout);
  }

  void setIcon(const ImageURL &url) {
    ImageURL finalUrl = url;

    finalUrl.setFill(SemanticColor::TextMuted);
    m_icon->setUrl(finalUrl);
  }
  void setTitle(const QString &title) { m_title->setText(title); }

  SettingsNavPane() { setupUI(); }

signals:
  void clicked() const;
};

class SettingsNavWidget : public QWidget {
  Q_OBJECT

  QHBoxLayout *m_layout = new QHBoxLayout;
  std::vector<QString> m_panes;

public:
  void setSelected(const QString &id) {
    for (int i = 0; i != m_panes.size(); ++i) {
      auto paneId = m_panes[i];
      SettingsNavPane *pane = static_cast<SettingsNavPane *>(m_layout->itemAt(i)->widget());

      if (paneId == id) {
        pane->select();
      } else {
        pane->deselect();
      }
    }
  }

  void addPane(const QString &id, const QString &title, const ImageURL &icon) {
    auto pane = new SettingsNavPane;
    size_t idx = m_panes.size();

    connect(pane, &SettingsNavPane::clicked, this, [this, idx]() { emit rowChanged(idx); });

    m_panes.emplace_back(id);
    pane->setTitle(title);
    pane->setIcon(icon);
    pane->setFixedWidth(100);
    m_layout->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(pane);
  }

public:
  SettingsNavWidget() { setLayout(m_layout); }

signals:
  void rowChanged(int index) const;
};

struct PaneInfo {
  QString title;
  ImageURL icon;
  QWidget *content = nullptr;
};

class SettingsWindow : public QMainWindow {
  ApplicationContext *m_ctx = nullptr;
  std::vector<std::unique_ptr<SettingsCategory>> m_categories;
  SettingsNavWidget *m_navigation = new SettingsNavWidget;
  QStackedWidget *content = new QStackedWidget;

  void showEvent(QShowEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  QWidget *createWidget();
  void keyPressEvent(QKeyEvent *) override;

public:
  SettingsWindow(ApplicationContext *ctx);
  ~SettingsWindow();
};
