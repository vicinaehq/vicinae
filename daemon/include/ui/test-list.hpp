#pragma once

#include "common.hpp"
#include "extend/metadata-model.hpp"
#include "ui/action_popover.hpp"
#include "ui/horizontal-metadata.hpp"
#include "ui/top_bar.hpp"
#include "ui/virtual-list.hpp"
#include <qabstractitemmodel.h>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qlist.h>
#include <qlistview.h>
#include <qlistwidget.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpainter.h>
#include <qstyleditemdelegate.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <qwidget.h>
#include <variant>

class AbstractNativeListItemDetail {
public:
  virtual QWidget *createView() const { return nullptr; };
  virtual MetadataModel createMetadata() const { return {}; }
};

class VContainer : public QWidget {
public:
  QVBoxLayout *layout;

  VContainer() : layout(new QVBoxLayout) {
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
  }
};

class TestDetailWidget : public QWidget {
  QVBoxLayout *layout;
  QWidget *view;
  HorizontalMetadata *metadata;
  HDivider *divider;

public:
  TestDetailWidget(const AbstractNativeListItemDetail &detail)
      : layout(new QVBoxLayout), view(detail.createView()), metadata(new HorizontalMetadata()),
        divider(new HDivider) {
    layout->addWidget(view, 1);
    layout->addWidget(divider);
    layout->addWidget(metadata);
    layout->setContentsMargins(0, 0, 0, 0);

    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    metadata->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setLayout(layout);

    auto detailMetadata = detail.createMetadata();

    for (const auto &child : detailMetadata.children) {
      metadata->addItem(child);
    }
  }
};

class AbstractNativeListItem : public AbstractVirtualListItem {
public:
  QList<AbstractAction *> actions;

  virtual std::unique_ptr<AbstractNativeListItemDetail> createDetail() const { return nullptr; }
  virtual std::unique_ptr<CompleterData> createCompleter() const { return nullptr; }

  // a unique role that differenciate two different kinds of list widget,
  // usually rendering a different widget. This determines whether the list
  // should call update or create a brand new widget on update.

  virtual QList<AbstractAction *> createActions() const { return {}; }

  QList<AbstractAction *> generateActions() {
    actions = createActions();

    return actions;
  }

public:
  virtual int height() const { return 40; }

  AbstractNativeListItem(size_t id = qHash(QUuid::createUuid())) : AbstractVirtualListItem(id) {}
  ~AbstractNativeListItem() {
    for (const auto &action : actions) {
      action->deleteLater();
    }
  }
};

struct ListSection {
  QString title;
  QList<std::shared_ptr<AbstractNativeListItem>> items;
};

using TestListItem = std::variant<ListSection, AbstractNativeListItem *>;

class AbstractTestListModel : public QObject {
  Q_OBJECT

  QList<TestListItem> m_items;
  QList<std::shared_ptr<AbstractNativeListItem>> flatItems;
  std::optional<ListSection> currentSection;

public:
  void beginReset() {
    m_items.clear();
    flatItems.clear();
    currentSection.reset();
  }

  void beginSection(const QString &name) {
    endSection();
    currentSection = {name};
  }

  void endSection() {
    if (!currentSection || currentSection->items.isEmpty()) return;

    m_items.push_back(*currentSection);
    currentSection.reset();
  }

  void addItem(const std::shared_ptr<AbstractNativeListItem> &item) {
    flatItems.push_back(item);

    if (currentSection) {
      currentSection->items.push_back(item);
      return;
    }

    m_items.push_back(item.get());
  }

  const QList<TestListItem> &items() const { return m_items; }

  const AbstractNativeListItem &row(int idx) { return *flatItems.at(idx).get(); }

  void endReset() {
    endSection();
    emit itemsChanged();
  }

signals:
  void itemsChanged();
  void itemDeleted(int idx);
};

class TestList : public QWidget {
  Q_OBJECT

  QHBoxLayout *splitter;
  QHash<QListWidgetItem *, AbstractNativeListItem *> itemMap;
  QListWidget *list;
  VContainer *detailContainer;
  AbstractTestListModel *model;

  void addListItem(AbstractNativeListItem *item) {
    auto itemWidget = new QListWidgetItem;
    auto widget = item->createItem();

    if (!widget) {
      qDebug() << "Could not create widget for native list: "
                  "delegate->createItem returned null";
      return;
    }

    list->addItem(itemWidget);
    list->setItemWidget(itemWidget, widget);
    itemWidget->setSizeHint(widget->sizeHint());
    itemMap.insert(itemWidget, item);
  }

private slots:
  void currentItemChanged(QListWidgetItem *next, QListWidgetItem *prev) {
    if (!itemMap.contains(next)) return;

    auto item = itemMap.value(next);

    if (auto detail = item->createDetail()) {
      auto detailWidget = new TestDetailWidget(*detail.get());

      if (detailContainer->layout->count() > 0) {
        QWidget *old = detailContainer->layout->itemAt(0)->widget();

        detailContainer->layout->replaceWidget(old, detailWidget);
        old->deleteLater();
      } else {
        detailContainer->layout->addWidget(detailWidget);
      }

      auto detailWidth = (width() / 3) * 2;
      qDebug() << "detail width" << detailWidth;
      detailContainer->setFixedWidth(detailWidth);
      detailContainer->show();
    }

    emit selectionChanged(*item);
  }

  void itemDeleted(int idx) {
    auto item = list->item(idx);

    if (auto widget = list->itemWidget(item)) widget->deleteLater();

    list->takeItem(idx);
  }

  void modelItemsChanged() {
    list->clear();

    for (const auto &item : model->items()) {
      if (auto section = std::get_if<ListSection>(&item)) {
        auto headerItem = new QListWidgetItem();

        headerItem->setFlags(headerItem->flags() & !Qt::ItemIsSelectable);
        list->addItem(headerItem);
        auto headerWidget = new ListSectionHeader(section->title, "", section->items.size());
        list->setItemWidget(headerItem, headerWidget);
        headerItem->setSizeHint(headerWidget->sizeHint());

        for (const auto &listItem : section->items) {
          addListItem(listItem.get());
        }
      }

      if (auto listItem = std::get_if<AbstractNativeListItem *>(&item)) { addListItem(*listItem); }
    }

    if (list->count() == 0) {
      QWidget *detail = splitter->itemAt(2)->widget();

      detailContainer->hide();
    }

    // list->setMinimumHeight(2000);
    // list->updateGeometry();

    for (int i = 0; i != list->count(); ++i) {
      auto item = list->item(i);

      if (!item->flags().testFlag(Qt::ItemIsSelectable)) continue;

      list->setCurrentItem(item);
      break;
    }
  }

public:
  void setModel(AbstractTestListModel *model) {
    this->model = model;
    connect(model, &AbstractTestListModel::itemsChanged, this, &TestList::modelItemsChanged);
    connect(model, &AbstractTestListModel::itemDeleted, this, &TestList::itemDeleted);
  }

  QListWidget *listWidget() { return list; }

  TestList()
      : model(nullptr), splitter(new QHBoxLayout), list(new QListWidget), detailContainer(new VContainer) {
    splitter->addWidget(list, 1);
    splitter->addWidget(new VDivider);
    splitter->addWidget(detailContainer, 2);

    detailContainer->hide();

    splitter->setSpacing(0);
    splitter->setContentsMargins(0, 0, 0, 0);

    list->setFocusPolicy(Qt::NoFocus);
    list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setLayout(splitter);
    connect(list, &QListWidget::currentItemChanged, this, &TestList::currentItemChanged);
  }

signals:
  void selectionChanged(const AbstractNativeListItem &item);
};
