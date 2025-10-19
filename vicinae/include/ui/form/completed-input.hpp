#pragma once
#include "common.hpp"
#include "ui/focus-notifier.hpp"
#include "ui/form/base-input.hpp"
#include "ui/image/image.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "ui/popover/popover.hpp"
#include <qlineedit.h>
#include <qwidget.h>

class CompletedInput : public JsonFormItemWidget {
public:
  class Completer {
  public:
    struct CompletionChoice {
      ImageURL url;
      QString title;
      QString subtitle;
    };

    class CompletionSection {
      QString m_title;
      std::vector<CompletionChoice> m_items;

    public:
      void addItem(const CompletionChoice &data) { m_items.emplace_back(data); }

      CompletionSection(const QString &title) : m_title(title) {}
    };

    using CompletionItem = std::variant<CompletionSection, CompletionChoice>;

    virtual std::vector<CompletionItem> generateCompletions(const QString &query) const = 0;
    virtual ~Completer() {}
  };
  class AbstractItem : public AbstractDefaultListItem {
  public:
    AbstractItem() {}

    virtual ImageURL icon() const { return ImageURL::builtin("circle"); };
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

  class CompletionListItem : public AbstractDefaultListItem {
    Completer::CompletionChoice m_data;

    ItemData data() const override {
      return {.iconUrl = m_data.url, .name = m_data.title, .accessories = {}};
    }

    QString generateId() const override { return m_data.title; }

  public:
    CompletionListItem(const Completer::CompletionChoice &data) : m_data(data) {}
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
  OmniList *m_completerList;
  BaseInput *inputField;
  ImageWidget *selectionIcon;
  Popover *popover;
  std::unique_ptr<AbstractItem> _currentSelection;

  bool eventFilter(QObject *obj, QEvent *event) override;
  void handleTextChanged(const QString &text);

public:
  CompletedInput(QWidget *parent = nullptr);
  ~CompletedInput();

  FocusNotifier *focusNotifier() const override;
  void clear();

  void setPlaceholderText(const QString &text);
  void setText(const QString &text);
  QString text() const;
  void setCompleter(std::unique_ptr<Completer> completer);

  void setValue(const QString &id);
  void setValueAsJson(const QJsonValue &value) override;
  OmniList *completer() { return m_completerList; }
  QLineEdit *input() { return inputField->input(); }
  void showCompleter() { showPopover(); }
  void hideCompleter() { popover->close(); }
  int cursorPosition() const { return inputField->input()->cursorPosition(); }

signals:
  void textChanged(const QString &s);
  void completionActivated(const OmniList::AbstractVirtualItem &item);

private slots:
  void showPopover();
};
