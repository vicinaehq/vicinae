#pragma once
#include "builtin_icon.hpp"
#include "ui/text-label.hpp"
#include "ui/omni-scroll-bar.hpp"
#include <qapplication.h>
#include <qboxlayout.h>
#include <qdatetime.h>
#include <qevent.h>
#include <qhash.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qscrollbar.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <quuid.h>
#include <qwidget.h>
#include <typeinfo>

enum GridMemberRole {
  GridSectionNameRole,
  GridItemRole,
};

class AbstractGridItemWidget : public QWidget {
  Q_OBJECT

public:
  virtual void selectionChanged(bool selected) {}

signals:
  void clicked();
  void doubleClicked();

public:
  AbstractGridItemWidget(QWidget *parent = nullptr) : QWidget(parent) {}
};

class AbstractGridMember : public QObject {
public:
  virtual AbstractGridItemWidget *widget(int columnWidth) const = 0;
  virtual bool update(AbstractGridItemWidget *dummy, int columnWidth) const { return false; }
  virtual int heightForWidth(int columnWidth) const = 0;
  virtual bool selectable() { return true; }
  virtual bool role() { return GridItemRole; }
  virtual int updateId() const { return -1; }
  virtual int key() const {
    qDebug() << "default key()";
    return qHash(QUuid::createUuid().toString());
  }

  AbstractGridMember() {}
  virtual ~AbstractGridMember() {}
};

class GridListSectionHeader : public AbstractGridItemWidget {
  TextLabel *countLabel;

public:
  GridListSectionHeader(const QString &title, const QString &subtitle, size_t count)
      : countLabel(new TextLabel) {
    setAttribute(Qt::WA_StyledBackground);

    auto layout = new QHBoxLayout();

    layout->setContentsMargins(5, 15, 5, 10);

    auto leftWidget = new QWidget();
    auto leftLayout = new QHBoxLayout();

    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(10);
    leftLayout->addWidget(new TextLabel(title));
    leftLayout->addWidget(countLabel);

    countLabel->setText(QString::number(count));
    leftWidget->setLayout(leftLayout);

    layout->addWidget(leftWidget, 0, Qt::AlignLeft | Qt::AlignVCenter);
    layout->addWidget(new TextLabel(subtitle), 0, Qt::AlignRight | Qt::AlignVCenter);

    setLayout(layout);
  }

  void setCount(int count) { countLabel->setText(QString::number(count)); }
};

struct GridSectionLabel : public AbstractGridMember {
  QString name;
  int count;

  AbstractGridItemWidget *widget(int columnWidth) const override {
    return new GridListSectionHeader(name, "", count);
  }

  int heightForWidth(int columnWidth) const override {
    GridListSectionHeader w(name, "", count);

    return w.sizeHint().height();
  }

  int key() const override { return qHash("section:" + name); }

  bool role() override { return GridSectionNameRole; }

  bool selectable() override { return false; }

  void setCount(int count) { this->count = count; }

  GridSectionLabel(const QString &name) : name(name), count(0) {}
};

class VirtualGridSection {

  QString m_name;
  int m_cols;
  int m_spacing;
  std::vector<QSharedPointer<AbstractGridMember>> m_items;

public:
  VirtualGridSection(const QString &name) : m_name(name), m_cols(1), m_spacing(0) {}

  const QString &name() const { return m_name; }
  int spacing() const { return m_spacing; }
  int columns() const { return m_cols; }
  const std::vector<QSharedPointer<AbstractGridMember>> &items() const { return m_items; }

  void setSpacing(int spacing) { m_spacing = spacing; }
  void setColumns(int columns) { m_cols = columns; }
  void setTitle(const QString &title) { m_name = title; }

  void addItem(const QSharedPointer<AbstractGridMember> &item) { m_items.push_back(item); }

  void addItem(AbstractGridMember *item) { m_items.push_back(QSharedPointer<AbstractGridMember>(item)); }

  void removeItem(AbstractGridMember *item) {
    for (int i = 0; i != m_items.size(); ++i) {
      if (m_items[i] == item) {
        m_items.erase(m_items.begin() + i);
        break;
      }
    }
  }
};

class GridItemWrapper : public QWidget {
  Q_OBJECT

  AbstractGridMember *item = nullptr;
  AbstractGridItemWidget *widget = nullptr;

  void resizeEvent(QResizeEvent *event) override {
    if (widget) { widget->setFixedSize(event->size()); }
    QWidget::resizeEvent(event);
  }

public:
  void setItem(AbstractGridMember *item) {
    this->widget = item->widget(maximumWidth());
    this->widget->setFixedSize(maximumSize());
    this->widget->setParent(this);
    this->item = item;

    connect(this->widget, &AbstractGridItemWidget::clicked, this, &GridItemWrapper::clicked);
    connect(this->widget, &AbstractGridItemWidget::doubleClicked, this, &GridItemWrapper::doubleClicked);
  }

  AbstractGridItemWidget *unwrap() const { return widget; }

  void setSelected(bool selected) { widget->selectionChanged(selected); }

  GridItemWrapper(QWidget *parent = nullptr) : QWidget(parent) {}

signals:
  void clicked();
  void doubleClicked();
};

class VirtualGridWidget : public QWidget {
  Q_OBJECT

  int SLIDER_FPS_CAP = 120;

  struct VirtualWidget {
    int key;
    int height;
    int offset;
    int widthOffset;
    int width;
    QSharedPointer<AbstractGridMember> item;
  };

  QTimer *sliderRatelimiter;

  QList<VirtualGridSection> m_sections;
  std::vector<VirtualWidget> m_virtual_items;

  int m_selected = -1;
  int m_selection_key = -1;
  struct {
    int left;
    int top;
    int bottom;
    int right;
  } margins;

  QScrollBar *scrollBar;
  QHash<int, GridItemWrapper *> visibleWidgets;
  QHash<int, GridItemWrapper *> widgetCache;
  QHash<QString, QSharedPointer<GridSectionLabel>> sectionLabelMap;
  QHash<int, QList<GridItemWrapper *>> widgetPool;

  void keyPressEvent(QKeyEvent *event) override {
    switch (event->key()) {
    case Qt::Key_Left:
      selectLeft();
      break;
    case Qt::Key_Right:
      selectRight();
      break;
    case Qt::Key_Up:
      selectTop();
      break;
    case Qt::Key_Down:
      selectBottom();
      break;
    case Qt::Key_Return:
      if (m_selected >= 0 && m_selected < m_virtual_items.size())
        emit itemActivated(*m_virtual_items[m_selected].item);
      break;
    }
  }

  void selectLeft() {
    for (int i = m_selected - 1; i >= 0; --i) {
      if (m_virtual_items[i].item->selectable()) {
        setSelected(i);
        return;
      }
    }
  }

  void selectRight() {
    for (int i = m_selected + 1; i < m_virtual_items.size(); ++i) {
      if (m_virtual_items[i].item->selectable()) {
        setSelected(i);
        return;
      }
    }
  }

  void selectTop() {
    if (m_selected == -1 || m_selected >= m_virtual_items.size()) return;

    auto current = m_virtual_items[m_selected];
    int nextRowStart = m_selected;

    while (nextRowStart >= 0) {
      auto item = m_virtual_items[nextRowStart];

      if (item.item->selectable() && item.offset < current.offset &&
          item.widthOffset <= current.widthOffset) {
        setSelected(nextRowStart);
        return;
      }

      --nextRowStart;
    }
  }

  void selectBottom() {
    if (m_selected == -1 || m_selected >= m_virtual_items.size()) return;

    int selected = m_selected;
    auto current = m_virtual_items[selected];
    int nextRowStart = selected;

    while (nextRowStart < m_virtual_items.size()) {
      while (nextRowStart < m_virtual_items.size() &&
             m_virtual_items[nextRowStart].offset == current.offset) {
        ++nextRowStart;
      }

      int nextRowEnd = nextRowStart;

      while (nextRowEnd < m_virtual_items.size() &&
             m_virtual_items[nextRowEnd].offset == m_virtual_items[nextRowStart].offset) {
        ++nextRowEnd;
      }

      int rowCount = nextRowEnd - nextRowStart;

      for (int i = 0; i != rowCount; ++i) {
        int index = nextRowEnd - i - 1;
        auto item = m_virtual_items[index];

        if (item.item->selectable() && item.widthOffset <= current.widthOffset) {
          setSelected(index);
          return;
        }
      }

      nextRowStart += rowCount;
    }
  }

  void clearVisibilityMap() {
    for (auto widget : visibleWidgets) {
      widget->hide();
    }
    visibleWidgets.clear();
  }

  void updateViewport() {
    for (auto widget : widgetCache) {
      widget->hide();
    }

    int scrollHeight = scrollBar->value();
    int startIndex = 0;

    while (startIndex < m_virtual_items.size()) {
      auto item = m_virtual_items[startIndex];

      if (item.offset + item.height >= scrollHeight) break;
      ++startIndex;
    }

    if (startIndex >= m_virtual_items.size()) return;

    int height = scrollHeight == 0 ? margins.top : m_virtual_items[startIndex].offset - scrollHeight;
    int endIndex = startIndex;

    int rowOffset = scrollHeight == 0 ? 0 : m_virtual_items[startIndex].offset;

    for (; endIndex < m_virtual_items.size(); ++endIndex) {
      auto vitem = m_virtual_items[endIndex];
      auto item = vitem.item;
      auto widget = widgetCache.value(vitem.key);
      int key = vitem.key;

      if (rowOffset < vitem.offset) {
        height += vitem.offset - rowOffset;
        rowOffset = vitem.offset;
      }

      if (!widget) {
        int upid = vitem.item->updateId();

        if (upid != -1) {
          auto &pool = widgetPool[upid];

          if (!pool.isEmpty()) {
            qDebug() << "pool of size" << pool.size();
            auto poolItem = pool[pool.size() - 1];

            if (item->update(poolItem->unwrap(), vitem.width)) {
              qDebug() << "updated widget from pool";
              widget = poolItem;
              pool.pop_back();
            }
          } else {
            qDebug() << "poule empty bordel";
          }
        }

        if (!widget) {
          widget = new GridItemWrapper(this);
          widget->setItem(item.get());
        }

        widgetCache.insert(vitem.key, widget);
      } else {
      }

      auto size = QSize{vitem.width, vitem.height};

      if (widget->size() != size) widget->setFixedSize({vitem.width, vitem.height});

      if (item->selectable()) {
        widget->disconnect();
        connect(widget, &GridItemWrapper::clicked, this, [this, endIndex]() {
          qDebug() << "clicked";
          setSelected(endIndex);
        });
        connect(widget, &GridItemWrapper::doubleClicked, this, [this, item]() { emit itemActivated(*item); });
      }

      visibleWidgets.insert(endIndex, widget);
      widget->setSelected(m_selected == endIndex);

      QPoint pos(vitem.widthOffset, height);

      if (pos != widget->pos()) { widget->move(pos); }
      if (!widget->isVisible()) widget->show();
      if (height > this->height()) break;
    }

    for (auto idx : visibleWidgets.keys()) {
      if (idx < startIndex || idx > endIndex) {
        auto vitem = m_virtual_items[idx];
        auto widget = visibleWidgets[idx];
        int upid = vitem.item->updateId();

        widgetCache.remove(vitem.key);
        visibleWidgets.remove(idx);

        if (upid != -1) {
          auto &pool = widgetPool[upid];
          qDebug() << "added to pool of size" << pool.size();
          pool << widget;
        } else {
          widget->deleteLater();
        }
      }
    }
  }

  void wheelEvent(QWheelEvent *event) override { QApplication::sendEvent(scrollBar, event); }

  void setSelected(int index) {
    if (index < 0) return;
    if (index >= m_virtual_items.size()) index = m_virtual_items.size() - 1;

    while (index < m_virtual_items.size() && !m_virtual_items[index].item->selectable()) {
      ++index;
    }

    if (index >= m_virtual_items.size()) return;

    auto item = m_virtual_items[index];
    int scrollHeight = scrollBar->value();

    m_selected = index;

    int previousRowEnd = index;

    while (previousRowEnd >= 0 && m_virtual_items[previousRowEnd].offset == item.offset) {
      --previousRowEnd;
    }

    if (previousRowEnd >= 0) {
      auto previousItem = m_virtual_items[previousRowEnd];

      if (previousItem.item->role() == GridSectionNameRole && previousItem.offset - scrollHeight < 0) {
        scrollBar->setValue(scrollHeight - (scrollHeight - item.offset) -
                            m_virtual_items[previousRowEnd].height);
        updateViewport();
        return;
      }
    }

    if (item.offset + item.height - scrollHeight > height()) {
      scrollBar->setValue(item.offset + item.height - height());
    } else if (item.offset - scrollHeight < 0) {
      scrollBar->setValue(scrollHeight - (scrollHeight - item.offset));
    }

    updateViewport();

    if (m_selection_key != item.key) {
      emit selectionChanged(*item.item);
      m_selection_key = item.key;
    }
  }

public:
  VirtualGridSection *section(const QString &key) {
    for (auto &section : m_sections) {
      if (section.name() == key) return &section;
    }

    m_sections << VirtualGridSection(key);

    return &m_sections[m_sections.size() - 1];
  }

  const QList<VirtualGridSection> &sections() { return m_sections; }

  void setMargins(int left, int top, int right, int bottom) {
    margins.left = left;
    margins.top = top;
    margins.right = right;
    margins.bottom = bottom;
  }

  void updateLayout() { calculateLayout(); }

  int indexOfItem(AbstractGridMember *item) {
    for (int i = 0; i != m_virtual_items.size(); ++i) {
      if (m_virtual_items[i].item == item) return i;
    }

    return -1;
  }

  bool removeSection(const QString &name) {
    for (int i = 0; i != m_sections.size(); ++i) {
      if (m_sections[i].name() == name) {
        sectionLabelMap.remove(name);
        m_sections.removeAt(i);

        return true;
      }
    }

    return false;
  }

  void invalidateCacheKey(int key) {
    if (auto widget = widgetCache.value(key)) {
      widget->deleteLater();
      widgetCache.remove(key);
      updateViewport();
    }
  }

  bool setSelectedKey(int key) {
    for (int i = 0; i != m_virtual_items.size(); ++i) {
      if (m_virtual_items[i].key == key) {
        setSelected(i);
        return true;
      }
    }

    return false;
  }

  void selectFirst() { setSelected(0); }

  int totalItemSize() {
    int count = 0;

    for (const auto &section : m_sections) {
      count += section.items().size();
    }

    return count;
  }

  bool isEmpty() {
    for (const auto &section : m_sections) {
      if (!section.items().empty()) return false;
    }

    return true;
  }

  void calculateLayout() {
    auto start = std::chrono::high_resolution_clock::now();

    int offset = 0;
    QHash<int, GridItemWrapper *> updatedCache;
    int size = totalItemSize();

    m_virtual_items.clear();
    m_virtual_items.reserve(size);

    int marginFreeWidth = width() - margins.left - margins.right;

    for (const auto &section : m_sections) {
      int innerGapSpacing = section.spacing() * (section.columns() - 1);
      int usableSpace = marginFreeWidth - innerGapSpacing;
      int idealColumnWidth = usableSpace / section.columns();
      int error = usableSpace - idealColumnWidth * section.columns();
      auto &items = section.items();
      int maxHeight = 0;

      if (!items.empty() && !section.name().isEmpty()) {
        auto labelItem = sectionLabelMap.value(section.name());

        if (!labelItem) {
          labelItem =
              QSharedPointer<GridSectionLabel>(new GridSectionLabel(section.name()), &QObject::deleteLater);
          sectionLabelMap.insert(section.name(), labelItem);
        }

        labelItem->setCount(items.size());

        auto key = labelItem->key();

        if (auto widget = widgetCache.value(key)) {
          auto labelWidget = static_cast<GridListSectionHeader *>(widget->unwrap());

          labelWidget->setCount(items.size());
          updatedCache.insert(key, widget);
        }

        int height = labelItem->heightForWidth(width() - margins.left - margins.right);

        VirtualWidget vitem{.key = key,
                            .height = height,
                            .offset = offset,
                            .widthOffset = margins.left,
                            .width = width() - margins.left - margins.right,
                            .item = labelItem};

        m_virtual_items.push_back(std::move(vitem));
        offset += height;
      }

      int widthOffset = margins.left;

      for (int i = 0; i != items.size(); ++i) {
        auto item = items.at(i);
        int colWidth = idealColumnWidth + (error < i % section.columns());
        int height = item->heightForWidth(colWidth);
        int key = item->key();

        if (auto widget = widgetCache.value(key)) { updatedCache.insert(key, widget); }

        VirtualWidget vitem{.key = key,
                            .height = height,
                            .offset = offset,
                            .widthOffset = widthOffset,
                            .width = colWidth,
                            .item = item};

        m_virtual_items.push_back(std::move(vitem));
        widthOffset += colWidth + section.spacing();
        maxHeight = std::max(height, maxHeight);

        if ((i + 1) % section.columns() == 0) {
          offset += maxHeight + section.spacing();
          widthOffset = margins.left;
          maxHeight = 0;
        }
      }

      if (items.size() % section.columns()) { offset += maxHeight; }
    }

    for (auto key : widgetCache.keys()) {
      if (!updatedCache.value(key)) { widgetCache.value(key)->deleteLater(); }
    }

    widgetCache = updatedCache;
    offset += margins.bottom;

    int virtualScrollHeight = std::max(0, offset - height());

    scrollBar->setMaximum(virtualScrollHeight);
    scrollBar->setVisible(virtualScrollHeight > 0);
    clearVisibilityMap();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    qDebug() << "computed layout in" << duration << "ms";

    setSelected(m_selected);
    updateViewport();
    emit layoutUpdated();
  }

  void clearContents() {
    m_selected = -1;

    sectionLabelMap.clear();
    clearVisibilityMap();
    scrollBar->setValue(0);

    m_virtual_items.clear();
    m_sections.clear();
  }

  AbstractGridItemWidget *itemWidget(AbstractGridMember *item) {
    int idx = indexOfItem(item);

    if (idx == -1) return nullptr;

    int key = m_virtual_items[idx].key;

    if (auto wrapper = widgetCache.value(key)) { return wrapper->unwrap(); }

    return nullptr;
  }

  void resizeEvent(QResizeEvent *event) override {
    scrollBar->setPageStep(height());
    scrollBar->setFixedSize(margins.right, height());
    scrollBar->move(width() - margins.right, 0);
    calculateLayout();
  }

  void handleScrollBarValueChanged() {
    if (scrollBar->isSliderDown()) return;

    qDebug() << "valueChanged";

    updateViewport();
  }

  void handleSliderMoved() {
    if (!sliderRatelimiter->isActive()) {
      sliderRatelimiter->start();
      updateViewport();
    }
  }

  VirtualGridWidget() : scrollBar(new OmniScrollBar(this)), sliderRatelimiter(new QTimer(this)) {
    sliderRatelimiter->setInterval(1000 / 120); // ~120 FPS

    connect(sliderRatelimiter, &QTimer::timeout, this, [this]() {
      updateViewport();
      sliderRatelimiter->stop();
    });

    setMargins(20, 10, 20, 10);
    scrollBar->hide();
    scrollBar->setMinimum(0);
    scrollBar->setSingleStep(40);
    connect(scrollBar, &QScrollBar::valueChanged, this, &VirtualGridWidget::handleScrollBarValueChanged);
    connect(scrollBar, &QScrollBar::sliderMoved, this, &VirtualGridWidget::handleSliderMoved);
  }

signals:
  void itemActivated(const AbstractGridMember &item);
  void selectionChanged(const AbstractGridMember &item);
  void layoutUpdated();
};
