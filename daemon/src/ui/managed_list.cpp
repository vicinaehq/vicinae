#include "ui/managed_list.hpp"
#include <QLabel>

ManagedList::ManagedList(QWidget *parent) : QListWidget(parent) {
  setProperty("class", "managed-list");
  setFocusPolicy(Qt::NoFocus);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  setSpacing(0);
  setContentsMargins(0, 0, 0, 0);

  connect(this, &QListWidget::currentItemChanged, this,
          &ManagedList::selectionChanged);
  connect(this, &QListWidget::itemActivated, this, &ManagedList::itemActivate);
}

void ManagedList::clear() {
  for (const auto &item : items) {
    delete item;
  }

  items.clear();
  sectionNames.clear();
  widgetToData.clear();
  QListWidget::clear();
}

void ManagedList::selectFirstEligible() {
  for (int i = 0; i != count(); ++i) {
    auto item = QListWidget::item(i);

    if (!item->flags().testFlag(Qt::ItemIsSelectable))
      continue;

    QListWidget::setCurrentItem(item);
    break;
  }
}

void ManagedList::addSection(const QString &name) {
  auto item = new QListWidgetItem(this);
  auto widget = new QLabel(name);

  widget->setContentsMargins(8, sectionNames.count() > 0 ? 10 : 0, 0, 10);
  item->setFlags(item->flags() & !Qt::ItemIsSelectable);
  widget->setProperty("class", "minor category-name");

  addItem(item);
  setItemWidget(item, widget);
  item->setSizeHint(widget->sizeHint());
  sectionNames.push_back(name);
}

void ManagedList::addWidgetItem(IActionnable *data, QWidget *widget) {
  auto item = new QListWidgetItem(this);

  addItem(item);
  setItemWidget(item, widget);
  item->setSizeHint(widget->sizeHint());
  widgetToData.insert(item, data);
}

void ManagedList::selectionChanged(QListWidgetItem *item,
                                   QListWidgetItem *previous) {
  if (auto it = widgetToData.find(item); it != widgetToData.end()) {
    emit itemSelected(**it);
  }
}

void ManagedList::itemActivate(QListWidgetItem *item) {
  if (auto it = widgetToData.find(item); it != widgetToData.end()) {
    emit itemActivated(**it);
  }
}
