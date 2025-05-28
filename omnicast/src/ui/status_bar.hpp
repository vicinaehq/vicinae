#pragma once
#include "common.hpp"
#include "extend/action-model.hpp"
#include "omni-icon.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/image/omnimg.hpp"
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
#include "ui/typography.hpp"

class StatusBar : public QWidget {
  Q_OBJECT

  QWidget *leftWidget;
  TypographyWidget *selectedActionLabel;
  QWidget *tmpLeft = nullptr;
  ShortcutButton *_selectedActionButton;
  ShortcutButton *_actionButton;
  VDivider *_rightDivider;
  QWidget *right;

  class CurrentCommandWidget : public QWidget {
    TypographyWidget *_title;
    Omnimg::ImageWidget *m_icon = new Omnimg::ImageWidget();

  public:
    CurrentCommandWidget(const QString &name, const OmniIconUrl &iconUrl) : _title(new TypographyWidget()) {
      auto layout = new QHBoxLayout();

      _title->setText(name);
      m_icon->setFixedSize(20, 20);
      m_icon->setUrl(iconUrl);

      layout->setContentsMargins(0, 0, 0, 0);
      layout->setSpacing(10);
      layout->addWidget(m_icon);
      layout->addWidget(_title);

      setLayout(layout);
    }

    const OmniIconUrl &icon() { return m_icon->url(); }
    QString title() const { return _title->text(); }
    void setTitle(const QString &title) const { _title->setText(title); }
    void setIcon(const OmniIconUrl &icon) { m_icon->setUrl(icon); }
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
  void setCurrentAction(const QString &action, const KeyboardShortcutModel &shortcut);
  void setActionButtonVisibility(bool value);
  void setCurrentActionButtonVisibility(bool value);
  void setActionButton(const QString &title, const KeyboardShortcutModel &shortcut);

  void clearAction();
  KeyboardShortcutModel actionButtonShortcut() const;
  void setActionButtonHighlight(bool highlight);
  void setToast(const QString &text, ToastPriority priority = ToastPriority::Success);
  void setNavigation(const QString &name, const OmniIconUrl &iconUrl);
  QString navigationTitle() const;
  OmniIconUrl navigationIcon() const;
  void setNavigationTitle(const QString &name);
  void setNavigationIcon(const OmniIconUrl &icon);
  void reset();

  StatusBar(QWidget *parent = nullptr);

signals:
  void currentActionButtonClicked() const;
  void actionButtonClicked() const;
};
