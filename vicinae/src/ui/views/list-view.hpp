#pragma once
#include "action-panel/action-panel.hpp"
#include "actions/theme/theme-actions.hpp"
#include "navigation-controller.hpp"
#include "simple-view.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "ui/search-bar/search-bar.hpp"
#include <absl/strings/str_format.h>
#include <qwidget.h>
#include <ranges>

class SplitDetailWidget;
class QStackedWidget;
class EmptyViewWidget;

class ListView : public SimpleView {
  SplitDetailWidget *m_split;
  QStackedWidget *m_content;
  EmptyViewWidget *m_emptyView;

public:
  struct Actionnable {
    virtual QList<AbstractAction *> generateActions() const { return {}; };
    virtual QWidget *generateDetail() const { return nullptr; }
    virtual std::unique_ptr<CompleterData> createCompleter() const { return nullptr; }
    virtual QString navigationTitle() const { return {}; }

    virtual std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const {
      return std::make_unique<ActionPanelState>();
    }

    virtual ActionPanelView *actionPanel() const {
      auto panel = new ActionPanelStaticListView;

      for (const auto &action : generateActions()) {
        panel->addAction(action);
      }

      return panel;
    }

    /**
     * Current action title to show in the status bar. Only shown if no primary action has been set.
     */
    virtual QString actionPanelTitle() const { return "Actions"; }

    // Whether to show the "Actions <action_shortcut>" next to the current action title
    bool showActionButton() const { return true; }
  };

protected:
  OmniList *m_list;

  virtual void itemSelected(const OmniList::AbstractVirtualItem *item);
  virtual bool inputFilter(QKeyEvent *event) override;

  virtual void selectionChanged(const OmniList::AbstractVirtualItem *next,
                                const OmniList::AbstractVirtualItem *previous);

  virtual void itemActivated(const OmniList::AbstractVirtualItem &item);

  QWidget *detail() const;

  void setDetail(QWidget *widget);

  void itemRightClicked(const OmniList::AbstractVirtualItem &item);

  void setupUI(QWidget *center);

public:
  ListView(QWidget *parent = nullptr);
};

/**
 * Simple list view handling search automatically
 * This offers less flexibility than the regular list view: this is mostly
 * meant for views where the dataset changes not so often and just needs to be
 * displayed and searched on.
 */
class SearchableListView : public ListView {
public:
  class Actionnable : public AbstractDefaultListItem, public ListView::Actionnable {
  public:
    virtual std::vector<QString> searchStrings() const = 0;
  };
  using ItemPtr = std::shared_ptr<Actionnable>;
  using Data = std::vector<ItemPtr>;
  using ScoreData = std::pair<ItemPtr, int>;

  void setData(Data items) {
    m_filtered.reserve(items.size());
    m_data = std::move(items);
  }

  virtual QString sectionName() const { return "Results ({count})"; }

  void render(const Data &filtered) {
    auto name = sectionName();
    name.replace("{count}", QString::number(filtered.size()));
    m_list->updateModel([&]() {
      auto &section = m_list->addSection(name);
      for (const auto &item : filtered) {
        section.addItem(item);
      }
    });
  }

  void initialize() final override {
    setData(initData());
    textChanged(searchText());
  }

  virtual Data initData() const = 0;

  const Data &filterData(const QString &query) {
    Data filtered;
    auto matches = [&](const QString &kw) { return kw.contains(query, Qt::CaseInsensitive); };
    auto anyMatches = [&](const ItemPtr &ss) { return std::ranges::any_of(ss->searchStrings(), matches); };

    m_filtered.clear();

    for (const auto &match : m_data | std::views::filter(anyMatches)) {
      m_filtered.emplace_back(match);
    }

    return m_filtered;
  }

  void textChanged(const QString &text) override {
    if (text.trimmed().isEmpty()) return render(m_data);
    return render(filterData(text));
  }

private:
  Data m_filtered;
  Data m_data;
};
