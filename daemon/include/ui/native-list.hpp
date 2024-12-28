
#pragma once

#include "common.hpp"
#include "extend/image-model.hpp"
#include "extend/list-model.hpp"
#include "omnicast.hpp"
#include "ui/list-view.hpp"
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
#include <tuple>
#include <variant>

class NativeListItemWidget : public QWidget {
  // QWidget *icon;
  QLabel *name;
  QLabel *category;
  QLabel *kind;

public:
  NativeListItemWidget(const QString &name, const QString &category,
                       const QString &kind, QWidget *parent = nullptr)
      : QWidget(parent), name(new QLabel), category(new QLabel),
        kind(new QLabel) {

    auto mainLayout = new QHBoxLayout();

    setLayout(mainLayout);

    auto left = new QWidget();
    auto leftLayout = new QHBoxLayout();

    this->name->setText(name);
    this->category->setText(category);
    this->category->setProperty("class", "minor");

    left->setLayout(leftLayout);
    leftLayout->setSpacing(15);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    // leftLayout->addWidget(this->icon);
    leftLayout->addWidget(this->name);
    leftLayout->addWidget(this->category);

    mainLayout->addWidget(left, 0, Qt::AlignLeft);

    this->kind->setText(kind);
    this->kind->setProperty("class", "minor");
    mainLayout->addWidget(this->kind, 0, Qt::AlignRight);
  }
};

struct NativeListItem {
  QString title;
  QString subtitle;
  ImageLikeModel icon;
};

/*

class NativeListModel : public QAbstractListModel {
  QList<NativeListItem> items;

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override {
    return "";
  }

  QVariant data(const QModelIndex &index, int role) const override {
    const NativeListItem &data = items.at(index.row());

    switch (role) {
    case Qt::UserRole:
      return QVariant::fromValue(data);
    default:
      return {};
    }
  }

  int rowCount(const QModelIndex &parent = QModelIndex()) const override {
    return items.size();
  }

public:
  void setItems(const QList<NativeListItem> &items) { this->items = items; }
};

class NativeListItemDelegate : public QStyledItemDelegate {
public:
  NativeListItemDelegate(QObject *parent = nullptr)
      : QStyledItemDelegate(parent) {}

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const override {
    auto item = index.data(Qt::UserRole).value<NativeListItem>();
    auto editor = new NativeListItemWidget(item.title, "", "");

    qDebug() << "created editor for" << item.title;

    return editor;
  }

  void setEditorData(QWidget *editor, const QModelIndex &index) const override {
  }

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override {
    auto item = index.data(Qt::UserRole).value<NativeListItem>();
    QBrush brush;

    brush.setColor("green");

    painter->setBackground(brush);
    painter->drawText(QPoint{0, 0}, item.title);
  }
};

class NativeList : public QListView {
  NativeListModel *model;
  NativeListItemDelegate *delegate;

public:
  NativeList()
      : model(new NativeListModel), delegate(new NativeListItemDelegate) {
    model->setItems({
        {"Item 1"},
        {"Item 2"},
        {"Item 3"},
        {"Item 4"},
        {"Item 5"},
        {"Item 6"},
    });

    setModel(model);
    setItemDelegate(delegate);
    show();
  }

  void currentChanged(const QModelIndex &current,
                      const QModelIndex &previous) override {
    qDebug() << "item changed";
  }
};
*/

class ListSectionHeader : public QWidget {
public:
  ListSectionHeader(const QString &title, const QString &subtitle,
                    size_t count) {
    auto layout = new QHBoxLayout();

    auto leftWidget = new QWidget();
    auto leftLayout = new QHBoxLayout();

    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(10);
    leftLayout->addWidget(new TextLabel(title));
    if (count > 0) {
      leftLayout->addWidget(new TextLabel(QString::number(count)));
    }
    leftWidget->setLayout(leftLayout);

    layout->addWidget(leftWidget, 0, Qt::AlignLeft | Qt::AlignVCenter);
    layout->addWidget(new TextLabel(subtitle), 0,
                      Qt::AlignRight | Qt::AlignVCenter);

    setLayout(layout);
  }
};

struct ListItemData {
  int role;
  QVariant data;
};

struct NativeListSection {
  QString title;
  QList<ListItemData> items;
};

using ListItem = std::variant<NativeListSection, ListItemData>;

class AbstractNativeListItemDelegate {
public:
  virtual QWidget *createDetail(const QVariant &data, int role) {
    return nullptr;
  }
  virtual QWidget *createItem(const QVariant &data, int role) = 0;
};

class AbstractNativeListModel : public QObject {
  Q_OBJECT

  QList<ListItem> items;
  std::optional<NativeListSection> currentSection;

public:
  void beginReset() { items.clear(); }

  const ListItem &at(int index) const { return items.at(index); }

  void beginSection(const QString &name) {
    endSection();
    currentSection = {name};
  }

  void endSection() {
    if (!currentSection || currentSection->items.isEmpty())
      return;

    items.push_back(*currentSection);
    currentSection.reset();
  }

  void addItem(const QVariant &data, int role) {
    if (currentSection) {
      currentSection->items.push_back({role, data});
      return;
    }

    items.push_back(ListItemData{role, data});
  }

  void endReset() {
    endSection();
    emit itemsChanged(this->items);
  }

signals:
  void itemsChanged(const QList<ListItem> &items);
};

template <class T>
class VariantNativeListItemDelegate : public AbstractNativeListItemDelegate {
  QWidget *createItem(const QVariant &data, int role) override {
    return createItemFromVariant(data.value<T>());
  }

  QWidget *createDetail(const QVariant &data, int role) override {
    return createDetailFromVariant(data.value<T>());
  }

public:
  virtual QWidget *createItemFromVariant(const T &variant) = 0;
  virtual QWidget *createDetailFromVariant(const T &variant) {
    return nullptr;
  };
};

template <class T>
class VariantNativeListModel : public AbstractNativeListModel {
  QList<T> items;

public:
  void addItem(const T &item) {
    AbstractNativeListModel::addItem(QVariant::fromValue(item), 0);
  }
};

class NativeList : public QWidget {
  Q_OBJECT

  AbstractNativeListItemDelegate *itemDelegate = nullptr;
  AbstractNativeListModel *model = nullptr;

  QHBoxLayout *splitter;
  QListWidget *list;
  QList<NativeListItem> row;

  void addListItem(const ListItemData &listItem) {
    auto itemWidget = new QListWidgetItem;
    auto widget = itemDelegate->createItem(listItem.data, listItem.role);

    list->addItem(itemWidget);
    list->setItemWidget(itemWidget, widget);
    itemWidget->setSizeHint(widget->sizeHint());
  }

private slots:
  void currentRowChanged(int currentRow) {
    if (currentRow < 0)
      return;

    /*
if (delegate->createDetail(item)) {
  qDebug() << "create detail";
}
    */
  }

  void modelItemsChanged(const QList<ListItem> &items) {
    list->clear();

    for (const auto &item : items) {
      if (auto section = std::get_if<NativeListSection>(&item)) {
        auto headerItem = new QListWidgetItem();

        headerItem->setFlags(headerItem->flags() & !Qt::ItemIsSelectable);
        list->addItem(headerItem);
        auto headerWidget =
            new ListSectionHeader(section->title, "", section->items.size());
        list->setItemWidget(headerItem, headerWidget);
        headerItem->setSizeHint(headerWidget->sizeHint());

        for (const auto &listItem : section->items) {
          addListItem(listItem);
        }
      }

      if (auto listItem = std::get_if<ListItemData>(&item)) {
        addListItem(*listItem);
      }
    }

    for (int i = 0; i != list->count(); ++i) {
      auto item = list->item(i);

      if (!item->flags().testFlag(Qt::ItemIsSelectable))
        continue;

      list->setCurrentItem(item);
      break;
    }
  }

public:
  void setItemDelegate(AbstractNativeListItemDelegate *delegate) {
    this->itemDelegate = delegate;
  }

  void setModel(AbstractNativeListModel *model) {
    this->model = model;
    connect(model, &AbstractNativeListModel::itemsChanged, this,
            &NativeList::modelItemsChanged);
  }

  NativeList()
      : itemDelegate(nullptr), model(nullptr), splitter(new QHBoxLayout),
        list(new QListWidget) {
    splitter->addWidget(list, 1);
    splitter->addWidget(new HDivider);
    splitter->setContentsMargins(0, 0, 0, 0);
    // splitter->addWidget(new QWidget, 2);

    list->setFocusPolicy(Qt::NoFocus);
    list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setLayout(splitter);
    connect(list, &QListWidget::currentRowChanged, this,
            &NativeList::currentRowChanged);
  }
};

template <class T> class VariantNativeList : public NativeList {
public:
  VariantNativeList() {}
  ~VariantNativeList() {}
};
