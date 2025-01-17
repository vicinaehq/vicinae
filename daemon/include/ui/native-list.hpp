
#pragma once

#include "common.hpp"
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

class ListSectionHeader : public QWidget {
public:
  ListSectionHeader(const QString &title, const QString &subtitle, size_t count) {
    setAttribute(Qt::WA_StyledBackground);

    auto layout = new QHBoxLayout();

    layout->setContentsMargins(8, 8, 8, 8);

    auto leftWidget = new QWidget();
    auto leftLayout = new QHBoxLayout();

    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(10);
    leftLayout->addWidget(new TextLabel(title));
    if (count > 0) { leftLayout->addWidget(new TextLabel(QString::number(count))); }
    leftWidget->setLayout(leftLayout);

    layout->addWidget(leftWidget, 0, Qt::AlignLeft | Qt::AlignVCenter);
    layout->addWidget(new TextLabel(subtitle), 0, Qt::AlignRight | Qt::AlignVCenter);

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
  virtual void selectionChanged(const QVariant &data) {}
  virtual QWidget *createDetail(const QVariant &data, int role) { return nullptr; }
  virtual QWidget *createItem(const QVariant &data, int role) = 0;
  virtual QList<QVariant> createActions(const QVariant &data) { return {}; }
};

class AbstractNativeListModel : public QObject {
  Q_OBJECT

  QList<ListItem> m_items;
  QList<ListItemData> flatItems;
  std::optional<NativeListSection> currentSection;

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

  bool removeItemIf(std::function<bool(const QVariant &variant)> predicate) {
    size_t idx = 0;

    for (size_t i = 0; i != m_items.size(); ++i) {
      auto &item = m_items[i];

      if (auto section = std::get_if<NativeListSection>(&item)) {
        ++idx;

        for (size_t i = 0; i != section->items.size(); ++i) {
          auto &item = section->items.at(i);

          if (!predicate(item.data)) {
            ++idx;
            continue;
          }

          section->items.removeAt(i);
          emit itemDeleted(idx);

          return true;
        }
      }

      if (auto data = std::get_if<ListItemData>(&item)) {
        if (!predicate(data->data)) {
          ++idx;
          continue;
        }

        m_items.removeAt(i);
        emit itemDeleted(idx);
        return true;
      }
    }

    return false;
  }

  void addItem(const QVariant &data, int role) {
    flatItems.push_back({.role = role, .data = data});

    if (currentSection) {
      currentSection->items.push_back({.role = role, .data = data});
      return;
    }

    m_items.push_back(ListItemData{.role = role, .data = data});
  }

  const QList<ListItem> &items() const { return m_items; }

  const ListItemData &row(int idx) { return flatItems.at(idx); }

  void endReset() {
    endSection();
    emit itemsChanged();
  }

signals:
  void itemsChanged();
  void itemDeleted(int idx);
};

template <class T> class TypedNativeListDelegate : public AbstractNativeListItemDelegate {
  QWidget *createItem(const QVariant &data, int role) override {
    return createItemFromVariant(data.value<T>());
  }

  void selectionChanged(const QVariant &data) override { return variantSelectionChanged(data.value<T>()); }

  QWidget *createDetail(const QVariant &data, int role) override {
    return createDetailFromVariant(data.value<T>());
  }

public:
  virtual QWidget *createItemFromVariant(const T &variant) = 0;
  virtual QWidget *createDetailFromVariant(const T &variant) { return nullptr; };
  virtual void variantSelectionChanged(const T &variant) {}
};

template <class T> class TypedNativeListModel : public AbstractNativeListModel {
public:
  void addItem(const T &item) { AbstractNativeListModel::addItem(QVariant::fromValue(item), 0); }

  /*
  void removeIf(std::function<bool(const T &item)> predicate) {
    AbstractNativeListModel::removeItemIf([predicate](const QVariant &variant) {
      return predicate(variant.value<T>());
    });
  }
  */
};

class NativeList : public QWidget {
  Q_OBJECT

  AbstractNativeListItemDelegate *itemDelegate = nullptr;
  AbstractNativeListModel *model = nullptr;

  QHBoxLayout *splitter;
  QHash<QListWidgetItem *, const ListItemData *> itemMap;
  QListWidget *list;

  void addListItem(const ListItemData &listItem) {
    auto itemWidget = new QListWidgetItem;
    auto widget = itemDelegate->createItem(listItem.data, listItem.role);

    if (!widget) {
      qDebug() << "Could not create widget for native list: "
                  "delegate->createItem returned null";
      return;
    }

    list->addItem(itemWidget);
    list->setItemWidget(itemWidget, widget);
    itemWidget->setSizeHint(widget->sizeHint());
    itemMap.insert(itemWidget, &listItem);
  }

private slots:
  void currentItemChanged(QListWidgetItem *next, QListWidgetItem *prev) {
    if (!itemMap.contains(next)) return;

    auto item = itemMap.value(next);

    itemDelegate->selectionChanged(item->data);

    if (auto detail = itemDelegate->createDetail(item->data, item->role)) {
      QWidget *old = splitter->itemAt(2)->widget();

      splitter->replaceWidget(old, detail);
      old->deleteLater();
    }
  }

  void itemDeleted(int idx) {
    auto item = list->item(idx);

    if (auto widget = list->itemWidget(item)) widget->deleteLater();

    list->takeItem(idx);
  }

  void modelItemsChanged() {
    list->clear();

    for (const auto &item : model->items()) {
      if (auto section = std::get_if<NativeListSection>(&item)) {
        auto headerItem = new QListWidgetItem();

        headerItem->setFlags(headerItem->flags() & !Qt::ItemIsSelectable);
        list->addItem(headerItem);
        auto headerWidget = new ListSectionHeader(section->title, "", section->items.size());
        list->setItemWidget(headerItem, headerWidget);
        headerItem->setSizeHint(headerWidget->sizeHint());

        for (const auto &listItem : section->items) {
          addListItem(listItem);
        }
      }

      if (auto listItem = std::get_if<ListItemData>(&item)) { addListItem(*listItem); }
    }

    for (int i = 0; i != list->count(); ++i) {
      auto item = list->item(i);

      if (!item->flags().testFlag(Qt::ItemIsSelectable)) continue;

      list->setCurrentItem(item);
      break;
    }
  }

public:
  void setItemDelegate(AbstractNativeListItemDelegate *delegate) { this->itemDelegate = delegate; }

  void setModel(AbstractNativeListModel *model) {
    this->model = model;
    connect(model, &AbstractNativeListModel::itemsChanged, this, &NativeList::modelItemsChanged);
    connect(model, &AbstractNativeListModel::itemDeleted, this, &NativeList::itemDeleted);
  }

  QListWidget *listWidget() { return list; }

  NativeList() : itemDelegate(nullptr), model(nullptr), splitter(new QHBoxLayout), list(new QListWidget) {
    splitter->addWidget(list, 1);
    splitter->addWidget(new VDivider);

    auto fakeDetails = new QWidget();

    fakeDetails->hide();

    splitter->addWidget(fakeDetails, 2);
    splitter->setSpacing(0);
    splitter->setContentsMargins(0, 0, 0, 0);

    list->setFocusPolicy(Qt::NoFocus);
    list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setLayout(splitter);
    connect(list, &QListWidget::currentItemChanged, this, &NativeList::currentItemChanged);
  }

signals:
  void selectionChanged(const QVariant &variant);
};

template <class T> class VariantNativeList : public NativeList {
public:
  VariantNativeList() {}
  ~VariantNativeList() {}
};
