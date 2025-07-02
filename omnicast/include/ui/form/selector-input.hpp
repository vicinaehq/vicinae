#pragma once
#include "common.hpp"
#include "libtrie/trie.hpp"
#include "omni-icon.hpp"
#include "ui/focus-notifier.hpp"
#include "ui/form/base-input.hpp"
#include "ui/horizontal-loading-bar.hpp"
#include "ui/omni-list.hpp"
#include "ui/popover.hpp"
#include <algorithm>
#include <memory>
#include <qjsonvalue.h>
#include <qlineedit.h>
#include <qobject.h>
#include <qstackedwidget.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class SelectorInput : public JsonFormItemWidget {
public:
  class AbstractItem : public AbstractDefaultListItem {
  public:
    AbstractItem() {}

    virtual std::optional<OmniIconUrl> icon() const { return std::nullopt; };
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

  struct DropdownSection {
    QString title;
    std::vector<std::shared_ptr<AbstractItem>> items;
    Trie<std::shared_ptr<OmniList::AbstractVirtualItem>> index;

    void buildIndex() {
      std::ranges::for_each(
          items, [&](auto &&item) { index.indexLatinText(item->displayName().toStdString(), item); });
    }

    std::vector<std::shared_ptr<OmniList::AbstractVirtualItem>> search(const QString &query) const {
      return index.prefixSearch(query.toStdString());
    }

    DropdownSection(const QString &title, const std::vector<std::shared_ptr<AbstractItem>> &items)
        : title(title), items(items) {
      buildIndex();
    }
  };

private:
  Q_OBJECT

  FocusNotifier *m_focusNotifier = new FocusNotifier(this);
  bool m_focused = false;
  bool m_defaultFilterEnabled = true;
  int POPOVER_HEIGHT = 300;

  void listHeightChanged(int height);

  std::vector<DropdownSection> m_sections;

protected:
  OmniList *m_list;
  BaseInput *inputField;
  QLineEdit *m_searchField;
  OmniIcon *collapseIcon;
  HorizontalLoadingBar *m_loadingBar = new HorizontalLoadingBar(this);
  OmniIcon *selectionIcon;
  Popover *popover;
  QStackedWidget *m_content = new QStackedWidget(popover);
  QWidget *m_emptyView = new QWidget(m_content);

  std::unique_ptr<AbstractItem> _currentSelection;

  bool eventFilter(QObject *obj, QEvent *event) override;

  using UpdateItemCallback = std::function<void(AbstractItem *item)>;

  struct UpdateItemPayload {
    QString icon;
    QString displayName;
  };

public:
  SelectorInput(QWidget *parent = nullptr);
  ~SelectorInput();

  void addSection(const QString &title, const std::vector<std::shared_ptr<AbstractItem>> &items) {
    m_sections.emplace_back(DropdownSection(title, items));
  }

  void resetModel() { m_sections.clear(); }

  void updateModel() {
    m_list->updateModel([&]() {
      for (const auto &section : m_sections) {
        auto &sec = m_list->addSection(section.title);

        sec.addItems(section.items |
                     std::views::transform(
                         [](auto &&v) -> std::shared_ptr<OmniList::AbstractVirtualItem> { return v; }) |
                     std::ranges::to<std::vector>());
      }
    });
  }

  FocusNotifier *focusNotifier() const override;
  void setIsLoading(bool value);
  void clear();
  OmniList *list() const { return m_list; }

  void updateItem(const QString &id, const UpdateItemCallback &cb);
  const AbstractItem *value() const;
  bool setValue(const QString &id);
  void setValueAsJson(const QJsonValue &value) override;
  QString searchText();
  void setEnableDefaultFilter(bool value);
  void openSelector() { showPopover(); }

signals:
  void textChanged(const QString &s);
  void selectionChanged(const AbstractItem &item);

private slots:
  void handleTextChanged(const QString &text);
  void itemActivated(const OmniList::AbstractVirtualItem &vitem);
  void itemUpdated(const OmniList::AbstractVirtualItem &item);
  void showPopover();
};
