#pragma once
#include "ui/text-label.hpp"
#include <QWidget>
#include <qevent.h>
#include <qpainterpath.h>
#include <QVBoxLayout>
#include <qscrollbar.h>
#include <qscrollarea.h>
#include <qpainter.h>
#include <qframe.h>
#include <qtimer.h>
#include <quuid.h>

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

static const int SCROLL_GAP = 5;

class AbstractVirtualListItem {
  size_t m_id;

public:
  virtual QWidget *createItem() const = 0;
  virtual QWidget *updateItem(QWidget *widget) const { return createItem(); };
  virtual bool isSelectable() const { return true; }
  virtual int height() const = 0;
  virtual size_t id() const {
    size_t id = qHash(QUuid::createUuid());

    return id;
  }

  AbstractVirtualListItem() {}
};

struct VirtualListSection {
  QString name;
  QList<std::shared_ptr<AbstractVirtualListItem>> items;
};

using VirtualListChild = std::variant<VirtualListSection, std::shared_ptr<AbstractVirtualListItem>>;

struct VirtualListItem {
  int offset;
  std::shared_ptr<AbstractVirtualListItem> item;
};

class SectionLabelListItem : public AbstractVirtualListItem {
  QString section;
  int count;

  QWidget *createItem() const override { return new ListSectionHeader(section, "", count); }

  bool isSelectable() const override { return false; }

  int height() const override { return 30; }

public:
  void setCount(int count) { this->count = count; }

  SectionLabelListItem(const QString &section) : section(section) {}
};

class VirtualListModel : public QObject {
  Q_OBJECT

  using Item = std::shared_ptr<AbstractVirtualListItem>;

  struct Section {
    QString name;
    QList<Item> items;
  };

  std::optional<Section> currentSection;
  QList<Item> items;

signals:
  void commitReset(const QList<Item> &items);
  void selectionChanged(const Item &item);

public:
  void beginReset() { items.clear(); }

  void endReset() {
    endSection();
    emit commitReset(items);
  }

  void beginSection(const QString &name) {
    endSection();
    currentSection = {.name = name};
  }

  void endSection() {
    if (currentSection) {
      if (!currentSection->items.isEmpty()) {
        auto labelItem = std::make_shared<SectionLabelListItem>(currentSection->name);

        labelItem->setCount(currentSection->items.size());
        items << labelItem << currentSection->items;
      }

      currentSection.reset();
    }
  }

  void setSelected(const std::shared_ptr<AbstractVirtualListItem> &item) { emit selectionChanged(item); }

  void addItem(const std::shared_ptr<AbstractVirtualListItem> &item) {
    if (currentSection) {
      currentSection->items << item;
      return;
    }

    items << item;
  }

  VirtualListModel() {}
};

class VirtualListItemWidget : public QFrame {
  Q_OBJECT

  QWidget *widget = nullptr;
  bool selectable = true;
  bool isSelected = false;
  bool isHovered = false;

  void resizeEvent(QResizeEvent *event) override {
    if (widget) widget->setFixedSize(event->size());
  }

  void enterEvent(QEnterEvent *event) override { emit hovered(true); }
  void leaveEvent(QEvent *event) override { emit hovered(false); }

  void mousePressEvent(QMouseEvent *event) override {
    if (!selectable) return;
    if (event->button() == Qt::LeftButton) { emit selected(); }
  }

  void mouseDoubleClickEvent(QMouseEvent *event) override {
    if (!selectable) return;
    if (event->button() == Qt::LeftButton) { emit leftDoubleClick(); }
  }

  void paintEvent(QPaintEvent *) override {
    if (isSelected || isHovered) {
      int borderRadius = 10;
      QPainter painter(this);

      painter.setRenderHint(QPainter::Antialiasing, true);

      QPainterPath path;
      path.addRoundedRect(rect(), borderRadius, borderRadius);

      painter.setClipPath(path);

      QColor backgroundColor("#282726");

      painter.fillPath(path, backgroundColor);
    }
  }

public:
  VirtualListItemWidget(QWidget *parent = nullptr) : QFrame(parent) {}

  void setSelectable(bool selectable = true) { this->selectable = selectable; }

  void setSelected(bool selected) {
    if (!selectable) return;

    isSelected = selected;
    repaint();
  }

  void setHovered(bool hovered) {
    if (!selectable) return;

    isHovered = hovered;
    repaint();
  }

  void setWidget(QWidget *w) {
    widget = w;
    w->setParent(this);
    w->setFixedSize(size());
  }

signals:
  void hovered(bool flag);
  void selected();
  void leftDoubleClick();
};

class VirtualListContainer : public QFrame {
  Q_OBJECT

public:
  QVBoxLayout *layout;
  QWidget *spacer;

  VirtualListContainer() : layout(new QVBoxLayout), spacer(new QWidget) {
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignTop);
    layout->setSpacing(0);
    layout->addWidget(spacer);
    spacer->setFixedHeight(0);
    setLayout(layout);
  }
};

class VirtualListWidget : public QScrollArea {
  Q_OBJECT

  QScrollBar *scrollBar;
  QMap<int, VirtualListItemWidget *> visibleWidgets;
  QList<VirtualListItem> items;
  int currentSelectionIndex = 0;
  VirtualListContainer *container;
  VirtualListModel *model = nullptr;

private:
  void valueChanged(int value) { updateVisibleItems(value, height()); }

  void resizeEvent(QResizeEvent *event) override {
    updateVisibleItems(scrollBar->value(), event->size().height());
  }

  int getPreviousSelected() {
    if (currentSelectionIndex == 0) return currentSelectionIndex;

    int previous = currentSelectionIndex - 1;

    while (previous >= 0 && !items.at(previous).item->isSelectable())
      --previous;

    return previous;
  }

  int getNextSelected() {
    if (currentSelectionIndex == items.size() - 1) return currentSelectionIndex;

    int next = currentSelectionIndex + 1;

    while (currentSelectionIndex < items.size() && !items.at(next).item->isSelectable())
      ++next;

    return next;
  }

  void selectItem(int selectionIndex) {
    if (selectionIndex < 0 || selectionIndex >= items.size()) return;

    qDebug() << "selecting" << selectionIndex;

    if (auto previousItem = visibleWidgets.value(currentSelectionIndex)) { previousItem->setSelected(false); }

    if (auto nextItem = visibleWidgets.value(selectionIndex)) { nextItem->setSelected(true); }

    auto &newItem = items.at(selectionIndex);
    auto scrollHeight = verticalScrollBar()->value();

    currentSelectionIndex = selectionIndex;

    if (newItem.offset <= scrollHeight || newItem.offset - scrollHeight < newItem.item->height()) {
      auto distance = scrollHeight - newItem.offset;

      qDebug() << distance;
      scrollBar->setValue(scrollHeight - distance - newItem.item->height() - SCROLL_GAP);
      qDebug() << "scroll up";
      // scroll up
    } else if ((newItem.offset - scrollHeight) > (height() - newItem.item->height())) {
      // scroll down
      auto distance = abs(newItem.offset - scrollHeight - height());
      scrollBar->setValue(scrollHeight + distance + SCROLL_GAP);

      qDebug() << distance;
      qDebug() << "scroll down";
    }

    emit selectionChanged(items.at(selectionIndex).item);
  }

  void updateVisibleItems(int y, int height) {
    if (items.isEmpty()) return;

    int startIndex = 0;

    while (startIndex < items.size() - 1 && items.at(startIndex).offset < y)
      ++startIndex;

    int endIndex = startIndex;
    int startOffset = items.at(endIndex).offset;

    while (endIndex < items.size() - 1 && items.at(endIndex).offset - startOffset < height)
      ++endIndex;

    qDebug() << "items" << items.size();

    qDebug() << "range" << startIndex << "to" << endIndex;
    qDebug() << "start" << startIndex << "end" << endIndex;

    if (startIndex > 1) {
      container->spacer->setFixedHeight(items.at(startIndex - 1).offset);
    } else {
      container->spacer->setFixedHeight(0);
    }

    qDebug() << "spacer" << startOffset;

    for (auto idx : visibleWidgets.keys()) {
      if (idx < startIndex || idx > endIndex) {
        auto widget = visibleWidgets.value(idx);

        container->layout->removeWidget(widget);
        widget->deleteLater();
        visibleWidgets.remove(idx);
      }
    }

    for (int i = startIndex; i <= endIndex; ++i) {
      if (auto widget = visibleWidgets.value(i)) { continue; }

      auto &item = items.at(i).item;
      auto widget = item->createItem();

      if (!widget) continue;

      auto listItemWidget = new VirtualListItemWidget(container);

      listItemWidget->setSelectable(item->isSelectable());

      connect(listItemWidget, &VirtualListItemWidget::selected, this, [this, i]() { selectItem(i); });
      connect(listItemWidget, &VirtualListItemWidget::leftDoubleClick, this,
              [this, item]() { itemActivated(item); });
      connect(listItemWidget, &VirtualListItemWidget::hovered, this,
              [this, i, listItemWidget](bool hovered) { listItemWidget->setHovered(hovered); });

      listItemWidget->setFixedHeight(item->height());
      listItemWidget->setWidget(widget);

      if (currentSelectionIndex == i) listItemWidget->setSelected(true);

      container->layout->insertWidget(i - startIndex + 1, listItemWidget);
      visibleWidgets.insert(i, listItemWidget);
    }

    QTimer::singleShot(0, [this]() {
      for (auto widget : visibleWidgets) {
        auto hovered = widget->rect().contains(widget->mapFromGlobal(QCursor::pos()));

        widget->setHovered(hovered);
      }
    });
  }

  void keyPressEvent(QKeyEvent *keyEvent) override {
    if (keyEvent->modifiers() != Qt::Modifiers{}) return;

    switch (keyEvent->key()) {
    case Qt::Key_Up:
      selectItem(getPreviousSelected());
      return;
    case Qt::Key_Down:
      selectItem(getNextSelected());
      return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
      activateSelectedItem();
      return;
    default:
      break;
    }

    QScrollArea::keyPressEvent(keyEvent);
  }

  void activateSelectedItem() {
    qDebug() << "Activate selected";
    if (currentSelectionIndex >= items.size()) return;

    emit itemActivated(items.at(currentSelectionIndex).item);
  }

  void clear() {
    while (container->layout->count() > 1) {
      if (auto item = container->layout->takeAt(1)) { item->widget()->deleteLater(); }
    }
    visibleWidgets.clear();
  }

  void setItems(const QList<std::shared_ptr<AbstractVirtualListItem>> &items) {
    QList<VirtualListItem> virtualItems;
    int offset = 0;

    qDebug() << "clear items";

    clear();

    for (auto &item : items) {
      virtualItems.push_back({.offset = offset, .item = item});
      offset += item->height();
    }

    this->items = virtualItems;
    scrollBar->setMaximum(offset);

    if (verticalScrollBar()->value() > 0) {
      verticalScrollBar()->setValue(0);
    } else {
      updateVisibleItems(0, height());
    }

    int firstSelected = 0;

    while (firstSelected < virtualItems.size() && !virtualItems.at(firstSelected).item->isSelectable()) {
      ++firstSelected;
    }

    selectItem(firstSelected);
  }

public:
  VirtualListWidget(QWidget *parent = nullptr)
      : QScrollArea(parent), container(new VirtualListContainer), scrollBar(verticalScrollBar()) {
    setFrameShape(QFrame::NoFrame);
    setFocusPolicy(Qt::NoFocus);
    setWidgetResizable(true);
    setWidget(container);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    container->setAutoFillBackground(false);

    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &VirtualListWidget::valueChanged);
  }

  void setSelected(size_t id) {
    for (size_t i = 0; i != items.size(); ++i) {
      if (items.at(i).item->id() == id) {
        selectItem(i);
        break;
      }
    }
  }

  void setModel(VirtualListModel *newModel) {
    if (model) model->deleteLater();
    model = newModel;
    connect(model, &VirtualListModel::commitReset, this, &VirtualListWidget::setItems);
    /*
connect(model, &VirtualListModel::selectionChanged, this, [this](const auto& selected){
    });
    */
  }

signals:
  void selectionChanged(const std::shared_ptr<AbstractVirtualListItem> &);
  void itemActivated(const std::shared_ptr<AbstractVirtualListItem> &);
};
