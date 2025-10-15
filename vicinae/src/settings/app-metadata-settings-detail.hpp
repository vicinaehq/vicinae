#pragma once
#include "services/app-service/abstract-app-db.hpp"
#include "common.hpp"
#include "ui/typography/typography.hpp"
#include "utils/utils.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qplaintextedit.h>
#include <qsizepolicy.h>
#include <qwidget.h>

class MetadataRowWidget : public QWidget {
  TypographyWidget *m_name = new TypographyWidget;
  QWidget *m_widget = new QWidget;
  QHBoxLayout *m_layout = new QHBoxLayout;

public:
  void setLabel(const QString &text) { m_name->setText(text); }
  void setWidget(QWidget *widget) {
    if (auto item = m_layout->itemAt(1)) {
      if (auto previous = item->widget()) {
        previous->deleteLater();
        m_layout->replaceWidget(previous, widget);
      }
    }
  }

  void setText(const QString &text) {
    auto typo = new TypographyWidget;

    typo->setText(text);
    setWidget(typo);
  }

  MetadataRowWidget(QWidget *parent = nullptr) : QWidget(parent) {
    m_name->setColor(SemanticColor::TextMuted);
    m_name->setFontWeight(QFont::Weight::DemiBold);

    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_name, 0, Qt::AlignLeft | Qt::AlignHCenter);
    m_layout->addWidget(m_widget, 0, Qt::AlignRight | Qt::AlignHCenter);
    setLayout(m_layout);
  }
};

class AppMetadataSettingsDetail : public QWidget {
  std::shared_ptr<AbstractApplication> m_app;
  MetadataRowWidget *m_description = new MetadataRowWidget(this);
  TypographyWidget *m_descriptionParagraph = new TypographyWidget(this);
  MetadataRowWidget *m_id = new MetadataRowWidget(this);
  MetadataRowWidget *m_name = new MetadataRowWidget(this);
  MetadataRowWidget *m_where = new MetadataRowWidget(this);
  MetadataRowWidget *m_terminal = new MetadataRowWidget(this);

  void resizeEvent(QResizeEvent *event) override { QWidget::resizeEvent(event); }

  void setupUI(const std::shared_ptr<AbstractApplication> &app) {
    auto layout = new QVBoxLayout;

    m_descriptionParagraph->setWordWrap(true);

    m_id->setLabel("ID");
    m_id->setText(app->id());
    m_name->setLabel("Name");
    m_name->setText(app->displayName());
    m_where->setLabel("Where");
    m_where->setText(compressPath(app->path().parent_path()).c_str());

    if (auto description = app->description(); !description.isEmpty()) {
      m_description->setLabel("Description");
      m_descriptionParagraph->setText(description);
      layout->addWidget(m_description);
      layout->addWidget(m_descriptionParagraph);
      layout->addWidget(new HDivider);
    }

    layout->addWidget(m_id);
    layout->addWidget(new HDivider);
    layout->addWidget(m_name);
    layout->addWidget(new HDivider);
    layout->addWidget(m_where);

    m_terminal->setLabel("Opens in terminal");
    m_terminal->setText(app->isTerminalApp() ? "Yes" : "No");
    layout->addWidget(new HDivider);
    layout->addWidget(m_terminal);

    layout->addStretch();
    setLayout(layout);
  }

public:
  AppMetadataSettingsDetail(const std::shared_ptr<AbstractApplication> &app) : m_app(app) { setupUI(app); }
};
