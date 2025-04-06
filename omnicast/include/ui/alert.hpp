#pragma once
#include "theme.hpp"
#include "ui/button.hpp"
#include "ui/shortcut-button.hpp"
#include "ui/typography.hpp"
#include <qboxlayout.h>
#include <qdir.h>
#include <qlabel.h>
#include <qpainterpath.h>
#include <qwidget.h>

class ButtonGroup : public QWidget {
public:
  ButtonGroup(Button *left, Button *right, QWidget *parent = nullptr) : QWidget(parent) {
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(left);
    layout->addWidget(right);
    setLayout(layout);
  }
};

class AlertWidget : public QWidget {
  TypographyWidget *_title;
  TypographyWidget *_message;
  ShortcutButton *_cancelBtn;
  ShortcutButton *_actionBtn;

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
      : QWidget(parent), _title(new TypographyWidget(TextSize::TextTitle, ColorTint::TextPrimary)),
        _message(new TypographyWidget(TextSize::TextRegular, ColorTint::TextSecondary)),
        _cancelBtn(new ShortcutButton), _actionBtn(new ShortcutButton) {
    auto layout = new QVBoxLayout;

    _title->setText("Are you sure?");
    _message->setText("This action cannot be undone");
    _cancelBtn->setText("Cancel");
    _actionBtn->setText("Delete");

    layout->setContentsMargins(20, 20, 20, 20);
    layout->addWidget(_title);
    layout->addWidget(_message);
    layout->addWidget(new ButtonGroup(_cancelBtn, _actionBtn));
    setLayout(layout);
  }
};
