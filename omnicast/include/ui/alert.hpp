#pragma once
#include "theme.hpp"
#include "ui/omni-button.hpp"
#include "ui/typography.hpp"
#include <qboxlayout.h>
#include <qdir.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qpainterpath.h>
#include <qwidget.h>

class ButtonGroup : public QWidget {
public:
  ButtonGroup(OmniButtonWidget *left, OmniButtonWidget *right, QWidget *parent = nullptr) : QWidget(parent) {
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    layout->addWidget(left);
    layout->addWidget(right);
    setLayout(layout);
  }
};

class AlertWidget : public QWidget {
  TypographyWidget *_title;
  TypographyWidget *_message;
  OmniButtonWidget *_cancelBtn;
  OmniButtonWidget *_actionBtn;

  void paintEvent(QPaintEvent *event) override {
    auto &theme = ThemeService::instance().theme();
    int borderRadius = 10;
    QPainter painter(this);
    QPainterPath path;
    QPen pen(theme.colors.statusBackgroundBorder, 1);

    painter.setRenderHint(QPainter::Antialiasing, true);
    path.addRoundedRect(rect(), borderRadius, borderRadius);

    painter.setClipPath(path);

    QColor finalColor(theme.colors.statusBackground);

    finalColor.setAlphaF(0.98);
    painter.setPen(pen);
    painter.fillPath(path, finalColor);
    painter.drawPath(path);
  }

public:
  AlertWidget(QWidget *parent = nullptr)
      : QWidget(parent), _title(new TypographyWidget(TextSize::TextRegular, ColorTint::TextPrimary)),
        _message(new TypographyWidget(TextSize::TextRegular, ColorTint::TextSecondary)),
        _cancelBtn(new OmniButtonWidget), _actionBtn(new OmniButtonWidget) {
    auto layout = new QVBoxLayout;

    _title->setText("Are you sure?");
    _title->setFontWeight(QFont::Bold);

    _message->setText("This action cannot be undone");

    QMargins btnMargins(50, 8, 50, 8);

    _cancelBtn->setContentsMargins(btnMargins);
    _cancelBtn->setText("Cancel");
    _actionBtn->setContentsMargins(btnMargins);
    _actionBtn->setText("Delete");
    _actionBtn->setColor(ColorTint::Red);

    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(10);
    layout->addWidget(_title, 0, Qt::AlignCenter);
    layout->addWidget(_message, 0, Qt::AlignCenter);
    layout->addWidget(new ButtonGroup(_cancelBtn, _actionBtn));
    setLayout(layout);
  }
};
