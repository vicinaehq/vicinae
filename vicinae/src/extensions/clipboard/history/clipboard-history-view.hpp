#pragma once
#include "extensions/clipboard/history/clipboard-history-model.hpp"
#include "extensions/clipboard/history/clipboard-history-controller.hpp"
#include "ui/icon-button/icon-button.hpp"
#include "ui/preference-dropdown/preference-dropdown.hpp"
#include "ui/form/selector-input.hpp"
#include "ui/empty-view/empty-view.hpp"
#include "ui/views/typed-list-view.hpp"
#include <QtCore>
#include <qboxlayout.h>
#include <qevent.h>
#include <qfuturewatcher.h>
#include <qlabel.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qproperty.h>
#include <qstackedwidget.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <sys/socket.h>

class ClipboardStatusToolbar : public QWidget {
  Q_OBJECT

public:
  enum ClipboardStatus { Monitoring, Paused, Unavailable };

  TypographyWidget *m_text = new TypographyWidget;

  QWidget *m_right = new QWidget;
  TypographyWidget *m_rightText = new TypographyWidget;
  IconButton *m_rightIcon = new IconButton;
  ClipboardStatus m_status = ClipboardStatus::Unavailable;

  QString statusText(ClipboardStatus status) {
    switch (status) {
    case Monitoring:
      return "Pause clipboard";
    case Paused:
      return "Resume clipboard";
    default:
      return "Clipboard monitoring unavailable";
    }
  }

  ImageURL statusIcon(ClipboardStatus status) {
    switch (status) {
    case Monitoring:
      return ImageURL::builtin("pause-filled").setFill(SemanticColor::Orange);
    case Paused:
      return ImageURL::builtin("play-filled").setFill(SemanticColor::Green);
    default:
      return ImageURL::builtin("warning").setFill(SemanticColor::Red);
    }
  }

public:
  ClipboardStatus clipboardStatus() const { return m_status; }

  void setLeftText(const QString &text) { m_text->setText(text); }

  void setClipboardStatus(ClipboardStatus status) {
    m_rightText->setText(statusText(status));
    m_rightIcon->setUrl(statusIcon(status));
    m_status = status;
  }

  ClipboardStatusToolbar(QWidget *parent = nullptr) : QWidget(parent) {
    auto rightLayout = new QHBoxLayout;

    rightLayout->addWidget(m_rightText);
    rightLayout->addWidget(m_rightIcon);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    m_rightIcon->setFixedSize(25, 25);
    m_rightIcon->setUrl(ImageURL::builtin("pause-filled"));
    m_rightIcon->setBackgroundColor(Qt::transparent);
    m_rightIcon->setFocusPolicy(Qt::NoFocus);
    m_right->setLayout(rightLayout);
    m_rightText->setText("Pause clipboard");

    auto layout = new QHBoxLayout;

    layout->setContentsMargins(15, 8, 15, 8);
    m_text->setText("0 Items");

    layout->addWidget(m_text);
    layout->addWidget(m_right, 0, Qt::AlignRight | Qt::AlignVCenter);
    setLayout(layout);

    connect(m_rightIcon, &IconButton::clicked, this, &ClipboardStatusToolbar::statusIconClicked);
  }

signals:
  void statusIconClicked();
};

class ClipboardHistoryView : public TypedListView<ClipboardHistoryModel> {
public:
  enum class DefaultAction { Paste, Copy };

  ClipboardHistoryView();

  void initialize() override;
  void textChanged(const QString &value) override;

protected:
  std::unique_ptr<ActionPanelState> createActionPanel(const ItemType &item) const override;
  QWidget *generateDetail(const ItemType &item) const override;

private:
  static DefaultAction parseDefaultAction(const QString &str);

  void reloadCurrentSearch();
  void handleListFinished();
  QWidget *searchBarAccessory() const override { return m_filterInput; }
  QWidget *wrapUI(QWidget *m_content) override;
  void handleMonitoringChanged(bool monitor);
  void handleStatusClipboard();
  void handleFilterChange(const SelectorInput::AbstractItem &item);
  std::optional<QString> getSavedDropdownFilter();
  void saveDropdownFilter(const QString &value);

  ClipboardHistoryModel *m_model;
  ClipboardHistoryController *m_controller;
  ClipboardStatusToolbar *m_statusToolbar;
  PreferenceDropdown *m_filterInput = new PreferenceDropdown(this);
  DefaultAction m_defaultAction = DefaultAction::Copy;
};
