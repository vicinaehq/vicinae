#pragma once
#include "common.hpp"
#include "extend/action-model.hpp"
#include "extend/image-model.hpp"
#include "omni-icon.hpp"
#include "ui/action_popover.hpp"
#include "ui/text-label.hpp"
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
#include "ui/shortcut-button.hpp"

class StatusBar : public QWidget {
  Q_OBJECT

  QWidget *leftWidget;
  QLabel *selectedActionLabel;
  QWidget *tmpLeft = nullptr;
  ShortcutButton *_selectedActionButton;
  ShortcutButton *_actionButton;
  VDivider *_rightDivider;
  QWidget *right;

  class CurrentCommandWidget : public QWidget {
    TextLabel *_title;

  public:
    CurrentCommandWidget(const QString &name, const OmniIconUrl &iconUrl) : _title(new TextLabel(name)) {
      auto layout = new QHBoxLayout();
      auto icon = new OmniIcon();

      icon->setFixedSize(20, 20);
      icon->setUrl(iconUrl);

      layout->setContentsMargins(0, 0, 0, 0);
      layout->setSpacing(10);
      layout->addWidget(icon);
      layout->addWidget(_title);

      setLayout(layout);
    }

    QString title() const { return _title->text(); }
    void setTitle(const QString &title) const { _title->setText(title); }
  };

  class DefaultLeftWidget : public QWidget {
  public:
    DefaultLeftWidget() {
      auto leftLayout = new QHBoxLayout();
      auto icon = new OmniIcon;

      icon->setFixedSize(22, 22);
      icon->setUrl(BuiltinOmniIconUrl("omnicast"));
      leftLayout->addWidget(icon);
      leftLayout->setContentsMargins(0, 0, 0, 0);
      setLayout(leftLayout);
    }
  };

  void paintEvent(QPaintEvent *event) override;
  void setLeftWidget(QWidget *left);

public:
  void setAction(const AbstractAction &action);
  void clearAction();
  void setActionButtonHighlight(bool highlight);
  void setToast(const QString &text, ToastPriority priority = ToastPriority::Success);
  void setNavigation(const QString &name, const OmniIconUrl &iconUrl);
  QString navigationTitle() const;
  void setNavigationTitle(const QString &name);
  void reset();

  StatusBar(QWidget *parent = nullptr);

signals:
  void currentActionButtonClicked() const;
  void actionButtonClicked() const;
};
