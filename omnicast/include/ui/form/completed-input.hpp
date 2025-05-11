#pragma once
#include "common.hpp"
#include "ui/focus-notifier.hpp"
#include "ui/form/base-input.hpp"
#include "ui/horizontal-loading-bar.hpp"
#include "ui/omni-list.hpp"
#include "ui/popover.hpp"
#include <qwidget.h>

class CompletedInput : public QWidget, public IJsonFormField {
public:
  class Completer {
  public:
    struct CompletionData {
      OmniIconUrl url;
      QString title;
      QString subtitle;
    };

    virtual std::vector<CompletionData> generateCompletions(const QString &query) const = 0;
    virtual ~Completer() {}
  };
  class AbstractItem : public AbstractDefaultListItem {
  public:
    AbstractItem() {}

    virtual OmniIconUrl icon() const { return BuiltinOmniIconUrl("circle"); };
    virtual QString displayName() const = 0;
    virtual bool hasPartialUpdates() const override { return true; }

    /**
     * Once an item is selected a copy of it is stored as the current selection.
     * This is required to maintain an always correct selection even if the list
     * of available options changed (in case the list of options is dynamically generated
     * for instance)
     */
    virtual AbstractItem *clone() const = 0;

    ItemData data() const override { return {.iconUrl = icon(), .name = displayName()}; }
  };

  QJsonValue asJsonValue() const override;

private:
  Q_OBJECT

  std::unique_ptr<Completer> m_completer;
  FocusNotifier *m_focusNotifier = new FocusNotifier(this);
  bool m_focused = false;
  bool m_defaultFilterEnabled = true;
  int POPOVER_HEIGHT = 300;

protected:
  OmniList *m_list;
  BaseInput *inputField;
  HorizontalLoadingBar *m_loadingBar = new HorizontalLoadingBar(this);
  OmniIcon *selectionIcon;
  Popover *popover;
  std::unique_ptr<AbstractItem> _currentSelection;

  bool eventFilter(QObject *obj, QEvent *event) override;
  void handleTextChanged(const QString &text);

public:
  CompletedInput(QWidget *parent = nullptr);
  ~CompletedInput();

  FocusNotifier *focusNotifier() const;
  void setIsLoading(bool value);
  void clear();

  void setPlaceholderText(const QString &text);
  void setText(const QString &text);
  QString text() const;
  void setCompleter(std::unique_ptr<Completer> completer);

  void setValue(const QString &id);
  void setValueAsJson(const QJsonValue &value) override;

signals:
  void textChanged(const QString &s);
  void selectionChanged(const AbstractItem &item);

private slots:
  void itemActivated(const OmniList::AbstractVirtualItem &vitem);
  void showPopover();
};
