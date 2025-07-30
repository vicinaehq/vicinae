#pragma once
#include "simple-view.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/omni-list/omni-list.hpp"

class OmniGrid;
class QStackedWidget;
class EmptyViewWidget;

class GridView : public SimpleView {
  bool inputFilter(QKeyEvent *event) override;

public:
  struct Actionnable {
    virtual QList<AbstractAction *> generateActions() const { return {}; };
    virtual QString navigationTitle() const { return {}; }

    /**
     * Current action title to show in the status bar. Only shown if no primary action has been set.
     */
    virtual QString actionPanelTitle() const { return "Actions"; }
  };

  virtual QString rootNavigationTitle() const { return ""; }

  void onActivate() override;
  void applyActionnable(const Actionnable *actionnable);

protected:
  OmniGrid *m_grid;
  QStackedWidget *m_content;
  EmptyViewWidget *m_emptyView;

  virtual void selectionChanged(const OmniList::AbstractVirtualItem *next,
                                const OmniList::AbstractVirtualItem *previous);

  virtual void itemActivated(const OmniList::AbstractVirtualItem &item);

public:
  GridView(QWidget *parent = nullptr);
};
