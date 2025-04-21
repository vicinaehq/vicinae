#pragma once
#include "extend/empty-view-model.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/typography.hpp"
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qwidget.h>

class EmptyViewWidget : public QWidget {
  OmniIcon *m_icon = new OmniIcon(this);
  TypographyWidget *m_title = new TypographyWidget(this);
  TypographyWidget *m_description = new TypographyWidget(this);

  void setupUi() {
    auto layout = new QVBoxLayout();
    auto container = new QVBoxLayout();

    m_description->setColor(ColorTint::TextSecondary);
    m_icon->setFixedSize(48, 48);
    container->setAlignment(Qt::AlignCenter);
    layout->setSpacing(10);
    layout->addWidget(m_icon, 0, Qt::AlignCenter);
    layout->addWidget(m_title, 0, Qt::AlignCenter);
    layout->addWidget(m_description, 0, Qt::AlignCenter);
    container->addLayout(layout);
    setLayout(container);
  }

public:
  void setTitle(const QString &title) {
    m_title->setText(title);
    m_title->setVisible(!title.isEmpty());
  }

  void setDescription(const QString &description) {
    m_description->setText(description);
    m_description->setVisible(!description.isEmpty());
  }

  void setIcon(const std::optional<OmniIconUrl> url) {
    if (url) m_icon->setUrl(*url);
    m_icon->setVisible(url.has_value());
  }

  EmptyViewWidget(QWidget *parent = nullptr) : QWidget(parent) { setupUi(); }

  EmptyViewWidget(const EmptyViewModel &model, QWidget *parent = nullptr) : QWidget(parent) {
    setupUi();
    setIcon(model.icon);
    setTitle(model.title);
    setDescription(model.description);
  }
};
