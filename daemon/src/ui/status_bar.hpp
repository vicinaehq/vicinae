#pragma once
#include "extend/action-model.hpp"
#include "extend/image-model.hpp"
#include "image-viewer.hpp"
#include "ui/toast.hpp"
#include <QBoxLayout>
#include <QLabel>
#include <QWidget>
#include <qboxlayout.h>
#include <qdatetime.h>
#include <qhash.h>
#include <qicon.h>
#include <qlabel.h>
#include <qlogging.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class StatusBar : public QWidget {
  QWidget *leftWidget;
  QLabel *selectedActionLabel;
  QWidget *tmpLeft = nullptr;

  class CurrentCommandWidget : public QWidget {
  public:
    CurrentCommandWidget(const QString &name, const ImageLikeModel &model) {
      auto layout = new QHBoxLayout();
      auto iconLabel = new QLabel();

      layout->setContentsMargins(0, 0, 0, 0);
      layout->addWidget(ImageViewer::createFromModel(model, {25, 25}));
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
  void setCurrentAction(const ActionPannelItem &item);
  void setToast(const QString &text,
                ToastPriority priority = ToastPriority::Success);
  void setNavigationTitle(const QString &name, const ImageLikeModel &model);
  void reset();

  StatusBar(QWidget *parent = nullptr);
};
