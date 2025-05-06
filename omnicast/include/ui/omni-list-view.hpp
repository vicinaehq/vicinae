#pragma once
#include "extend/metadata-model.hpp"
#include "theme.hpp"
#include "ui/horizontal-metadata.hpp"
#include "ui/omni-list.hpp"
#include "view.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qnamespace.h>
#include <qwidget.h>

class OmniListView : public View {
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
    virtual std::vector<ActionItem> generateActionPannel() const { return {}; };
    virtual QWidget *generateDetail() const { return nullptr; }
    virtual std::unique_ptr<CompleterData> createCompleter() const { return nullptr; }
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

      if (auto completer = nextItem->createCompleter()) {
        app.topBar->activateQuicklinkCompleter(*completer);
      } else {
        app.topBar->destroyQuicklinkCompleter();
      }

      auto pannel = nextItem->generateActionPannel();

      if (!pannel.empty()) {
        setActionPannel(pannel);

        // if (auto first = app.actionPannel->actionnable(0)) { first->setShortcut({.key = "return"}); }
      } else {
        auto actions = nextItem->generateActions();

        if (!actions.isEmpty()) { actions.at(0)->setShortcut({.key = "return"}); }

        setSignalActions(actions);
      }
    } else {
      split->clearDetail();
      app.topBar->destroyQuicklinkCompleter();
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

  virtual void buildSearch(ItemList &list, const QString &s) {}

  void onSearchChanged(const QString &s) override {
    buildSearch(itemList, s);
    list->updateFromList(itemList, OmniList::SelectFirst);
    itemList.clear();
  }

public:
  OmniListView(AppWindow &app) : View(app), list(new OmniList) {
    split = new DetailSplit(list);
    connect(list, &OmniList::selectionChanged, this, &OmniListView::selectionChanged);
    connect(list, &OmniList::itemActivated, this, &OmniListView::itemActivated);

    auto layout = new QVBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(split);
    setLayout(layout);
  }
};
