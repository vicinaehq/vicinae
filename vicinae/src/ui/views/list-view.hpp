#pragma once
#include "action-panel/action-panel.hpp"
#include "navigation-controller.hpp"
#include "simple-view.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "ui/search-bar/search-bar.hpp"
#include <qwidget.h>

class SplitDetailWidget;
class QStackedWidget;
class EmptyViewWidget;

class ListView : public SimpleView {
  SplitDetailWidget *m_split;
  QStackedWidget *m_content;
  EmptyViewWidget *m_emptyView;

  virtual bool inputFilter(QKeyEvent *event) override;

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
