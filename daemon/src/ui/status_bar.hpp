#pragma once
#include "common.hpp"
#include <QBoxLayout>
#include <QLabel>
#include <QWidget>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qwidget.h>

class StatusBar : public QWidget {
  QWidget *leftWidget;
  QLabel *selectedActionLabel;

  class CurrentCommandWidget : public QWidget {
  public:
    CurrentCommandWidget(const QString &name, const QString &icon) {
      auto layout = new QHBoxLayout();
      auto iconLabel = new QLabel();

      layout->setContentsMargins(0, 0, 0, 0);
      iconLabel->setPixmap(QIcon::fromTheme(icon).pixmap(22, 22));
      layout->addWidget(iconLabel);
      layout->addWidget(new QLabel(name));

      setLayout(layout);
    }
  };

  class DefaultLeftWidget : public QWidget {
  public:
    DefaultLeftWidget() {
      auto leftLayout = new QHBoxLayout();

      auto leftIcon = new QLabel();
      QIcon::setThemeName("Papirus-Dark");
      leftIcon->setPixmap(
          QIcon::fromTheme(":/assets/icons/tux.svg").pixmap(22, 22));
      leftLayout->addWidget(leftIcon);
      leftLayout->setContentsMargins(0, 0, 0, 0);

      setLayout(leftLayout);
    }
  };

  void setLeftWidget(QWidget *left);

public:
  void setSelectedAction(const std::shared_ptr<IAction> &action);

  void setActiveCommand(const QString &name, const QString &icon);
  void reset();

  StatusBar(QWidget *parent = nullptr);
};
