#pragma once
#include "common.hpp"
#include "lib/fts_fuzzy.hpp"
#include "trie.hpp"
#include "../../../src/ui/image/url.hpp"
#include "ui/focus-notifier.hpp"
#include "ui/form/base-input.hpp"
#include "ui/loading-bar/horizontal-loading-bar.hpp"
#include "ui/image/image.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "ui/popover/popover.hpp"
#include <algorithm>
#include <memory>
#include <qjsonvalue.h>
#include <qlineedit.h>
#include <qobject.h>
#include <qstackedwidget.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <ranges>

class SelectorInput : public JsonFormItemWidget {
  Q_OBJECT

public:
  class AbstractItem : public AbstractDefaultListItem {
  public:
    AbstractItem() {}

    virtual std::optional<ImageURL> icon() const { return std::nullopt; };
    virtual QString displayName() const = 0;
    virtual bool hasPartialUpdates() const override { return true; }

    /**
     * Once an item is selected a copy of it is stored as the current selection.
     * This is required to maintain an always correct selection even if the list
     * of available options changed (in case the list of options is dynamically generated
     * for instance)
     */
    virtual AbstractItem *clone() const = 0;

    ItemData data() const override {
      return {.iconUrl = icon(), .name = displayName(), .secondaryBackground = true};
    }
  };

  QJsonValue asJsonValue() const override;

  struct ScoredItem {
    std::shared_ptr<AbstractItem> item;
    int score;
  };

  struct FuzzySearchResult {};

  struct DropdownSection {
    QString title;
    std::vector<std::shared_ptr<AbstractItem>> items;

    std::vector<ScoredItem> search(const QString &text) const {
      std::string pattern = text.toStdString();
      auto withScore = [&](auto &&item) {
        int fuzzyScore = 0;
        fts::fuzzy_match(pattern, item->displayName().toStdString(), fuzzyScore);
        return ScoredItem{item, fuzzyScore};
      };
      auto matches = [](auto &&item) { return item.score > 0; };
      auto results = items | std::views::transform(withScore) | std::views::filter(matches) |
                     std::ranges::to<std::vector>();
      std::ranges::stable_sort(results, [](auto &&a, auto &&b) { return a.score > b.score; });

      return results;
    }

    DropdownSection(const QString &title, const std::vector<std::shared_ptr<AbstractItem>> &items)
        : title(title), items(items) {}
  };

signals:
  void textChanged(const QString &s);
  void selectionChanged(const AbstractItem &item);

public:
  using UpdateItemCallback = std::function<void(AbstractItem *item)>;

  SelectorInput(QWidget *parent = nullptr);
  ~SelectorInput();

  void addSection(const QString &title, const std::vector<std::shared_ptr<AbstractItem>> &items);
  void resetModel();
  void updateModel();

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

protected:
  OmniList *m_list;
  BaseInput *inputField;
  QLineEdit *m_searchField;
  ImageWidget *collapseIcon;
  HorizontalLoadingBar *m_loadingBar = new HorizontalLoadingBar(this);
  ImageWidget *selectionIcon;
  Popover *popover;
  QStackedWidget *m_content = new QStackedWidget(popover);
  QWidget *m_emptyView = new QWidget(m_content);

  std::unique_ptr<AbstractItem> _currentSelection;

  bool eventFilter(QObject *obj, QEvent *event) override;

  struct UpdateItemPayload {
    QString icon;
    QString displayName;
  };

private slots:
  void handleTextChanged(const QString &text);
  void itemActivated(const OmniList::AbstractVirtualItem &vitem);
  void itemUpdated(const OmniList::AbstractVirtualItem &item);
  void showPopover();

private:
  FocusNotifier *m_focusNotifier = new FocusNotifier(this);
  bool m_focused = false;
  bool m_defaultFilterEnabled = true;
  int POPOVER_HEIGHT = 300;

  void listHeightChanged(int height);

  std::vector<DropdownSection> m_sections;
};
