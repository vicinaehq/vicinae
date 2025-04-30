#pragma once
#include "extend/metadata-model.hpp"
#include "theme.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/horizontal-metadata.hpp"
#include "ui/omni-list.hpp"
#include "view.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qnamespace.h>
#include <qwidget.h>

class DeclarativeOmniListView : public View {
  class DetailSplit : public QWidget {
    QWidget *_base;
    QWidget *_detail;
    bool _isPaneOpened;
    bool _dividerStrokeWidth = 1;

  protected:
    void recalculate() {
      double factor = _isPaneOpened ? 0.35 : 1;
      int baseWidth = width() * factor;
      int detailWidth = width() - baseWidth;

      _base->setFixedSize({baseWidth, height()});
      _base->move(0, 0);

      if (_detail) {
        _detail->setFixedSize({detailWidth, height()});
        _detail->move(baseWidth, 0);
        _detail->show();
      }
    }

    void resizeEvent(QResizeEvent *event) override {
      recalculate();
      QWidget::resizeEvent(event);
    }

    void paintEvent(QPaintEvent *event) override {
      auto &theme = ThemeService::instance().theme();
      QPainter painter(this);

      if (_isPaneOpened) {
        painter.setPen(theme.colors.border);
        painter.setBrush(theme.colors.border);
        painter.drawRect(_base->width(), 0, 1, height());
      }

      QWidget::paintEvent(event);
    }

  public:
    void setDetail(QWidget *detail) {
      if (_detail) { _detail->deleteLater(); }
      detail->setParent(this);
      _detail = detail;
      _isPaneOpened = true;
      recalculate();
    }

    void clearDetail() {
      _isPaneOpened = false;
      recalculate();
      if (_detail) {
        _detail->deleteLater();
        _detail = nullptr;
      }
    }

    DetailSplit(QWidget *base, QWidget *parent = nullptr)
        : QWidget(parent), _base(base), _detail(nullptr), _isPaneOpened(false) {
      base->setParent(this);
    }
  };

protected:
  DetailSplit *split;
  OmniList *list;
  QString lastSelectedId;

  using ItemList = std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>>;

  ItemList itemList;

public:
  class MetadataDetailModel {
  public:
    virtual QWidget *createView() const { return new QWidget; }
    virtual MetadataModel createMetadata() const { return {}; }
  };

  class SideDetailWidget : public QWidget {
    QVBoxLayout *layout;
    QWidget *view;
    HorizontalMetadata *metadata;
    HDivider *divider;

  public:
    SideDetailWidget(const MetadataDetailModel &detail)
        : layout(new QVBoxLayout), view(detail.createView()), metadata(new HorizontalMetadata()),
          divider(new HDivider) {
      layout->addWidget(view, 1);
      layout->addWidget(divider);
      layout->addWidget(metadata);
      layout->setContentsMargins(0, 0, 0, 0);
      layout->setSpacing(0);

      view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      metadata->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

      setLayout(layout);

      auto detailMetadata = detail.createMetadata();

      for (const auto &child : detailMetadata.children) {
        metadata->addItem(child);
      }
    }

    ~SideDetailWidget() { qDebug() << "detail destroyed"; }
  };

  class IActionnable {
  public:
    virtual QList<AbstractAction *> generateActions() const { return {}; };
    virtual QWidget *generateDetail() const { return nullptr; }
    virtual std::unique_ptr<CompleterData> createCompleter() const { return nullptr; }
    virtual std::vector<ActionItem> generateActionPannel() const { return {}; };
  };

protected:
  virtual void selectionChanged(const OmniList::AbstractVirtualItem *next,
                                const OmniList::AbstractVirtualItem *previous) {
    if (!next) {
      split->clearDetail();
      app.topBar->destroyQuicklinkCompleter();
      setSignalActions({});

      return;
    }

    if (auto nextItem = dynamic_cast<const IActionnable *>(next)) {
      if (auto detail = nextItem->generateDetail()) {
        split->setDetail(detail);
      } else {
        split->clearDetail();
      }

      if (auto completer = nextItem->createCompleter(); completer && completer->arguments.size() > 0) {
        app.topBar->activateCompleter(*completer);
      } else {
        app.topBar->destroyCompleter();
      }

      auto pannel = nextItem->generateActionPannel();

      if (!pannel.empty()) {
        int idx = 0;

        for (const auto &item : pannel) {
          if (auto action = std::get_if<std::unique_ptr<AbstractAction>>(&item)) {
            if (idx == 0) (*action)->setShortcut({.key = "return", .modifiers = {}});
            if (idx == 1) (*action)->setShortcut({.key = "return", .modifiers = {"shift"}});
            ++idx;
          }
        }

        setActionPannel(std::move(pannel));

        // if (auto first = app.actionPannel->actionnable(0)) { first->setShortcut({.key = "return"}); }
      } else {

        auto actions = nextItem->generateActions();

        if (!actions.isEmpty()) { actions.at(0)->setShortcut({.key = "return"}); }
        if (actions.size() > 1) { actions.at(1)->setShortcut({.key = "return", .modifiers = {"shift"}}); }

        setSignalActions(actions);
      }
    } else {
      split->clearDetail();
      app.topBar->destroyCompleter();
      setSignalActions({});
    }

    lastSelectedId = next->id();
  }

  virtual void itemActivated(const OmniList::AbstractVirtualItem &item) {
    qDebug() << "activated";
    emit activatePrimaryAction();
  }

  void recreateCurrentActions() {
    if (auto item = list->selected()) {
      if (auto nextItem = dynamic_cast<const IActionnable *>(item)) {
        setSignalActions(nextItem->generateActions());
      }
    }
  }

  bool inputFilter(QKeyEvent *event) override {
    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Return:
      QApplication::sendEvent(list, event);
      return true;
    }

    return View::inputFilter(event);
  }

  QString query;

  virtual ItemList generateList(const QString &s) = 0;

  void reload(OmniList::SelectionPolicy policy = OmniList::KeepSelection) {
    qDebug() << "reload list";
    auto items = generateList(query);

    list->invalidateCache();
    list->updateFromList(items, policy);

    /*
if (auto item = list->selected()) {
  if (auto nextItem = dynamic_cast<const IActionnable *>(item)) {
    setSignalActions(nextItem->generateActions());
  }
}
    */
  }

  void onActionActivated(const AbstractAction *action) override {
    if (isVisible()) {
      qDebug() << "action activated!";
      reload();
    } else {
      qDebug() << "no reload after action, as we are no longer visible!";
    }
  }

  void onRestore() override { /*reload(OmniList::KeepSelection);*/ }

  void onSearchChanged(const QString &s) override {
    query = s;
    auto items = generateList(s);
    list->updateFromList(items, OmniList::SelectFirst);
  }

public:
  DeclarativeOmniListView(AppWindow &app) : View(app), list(new OmniList) {
    split = new DetailSplit(list);
    connect(list, &OmniList::selectionChanged, this, &DeclarativeOmniListView::selectionChanged);
    connect(list, &OmniList::itemActivated, this, &DeclarativeOmniListView::itemActivated);

    auto layout = new QVBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(split);
    setLayout(layout);
  }
};
