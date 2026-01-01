#pragma once
#include "ui/typography/typography.hpp"
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
