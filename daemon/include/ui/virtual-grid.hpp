#pragma once
#include "builtin_icon.hpp"
#include "omni-icon.hpp"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qscrollbar.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class EllidedLabel : public QLabel {
  void paintEvent(QPaintEvent *event) override {
    QFrame::paintEvent(event);
    QPainter painter(this);
    auto metrics = fontMetrics();
    auto elided = metrics.elidedText(text(), Qt::ElideRight, width());

    painter.drawText(QPoint(0, metrics.ascent()), elided);
  }

public:
  EllidedLabel() { setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); }
};

class AbstractGridItem {
public:
  virtual QString title() const { return {}; }
  virtual QString subtitle() const { return {}; }
  virtual QString iconName() const = 0;
};

class MainWidget : public QWidget {
  Q_OBJECT

  QVBoxLayout *layout;
  OmniIcon *icon;
  bool m_selected;
  bool m_hovered;

protected:
  int borderWidth() const { return 3; }

  QColor borderColor() const {
    if (m_selected) return "#BBBBBB";
    if (m_hovered) return "#888888";

    return "#202020";
  }

  void paintEvent(QPaintEvent *event) override {
    int borderRadius = 10;

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath path;
    path.addRoundedRect(rect(), borderRadius, borderRadius);

    painter.setClipPath(path);

    QColor backgroundColor("#202020");

    painter.fillPath(path, backgroundColor);

    QPen pen(borderColor(), 3);
    painter.setPen(pen);
    painter.drawPath(path);
  }

  void enterEvent(QEnterEvent *event) override { setHovered(true); }
  void leaveEvent(QEvent *event) override { setHovered(false); }

  void mousePressEvent(QMouseEvent *event) override { emit clicked(); }
  void mouseDoubleClickEvent(QMouseEvent *event) override { emit doubleClicked(); }

public:
  MainWidget() : layout(new QVBoxLayout), icon(new OmniIcon), m_selected(false), m_hovered(false) {
    layout->addWidget(icon, 0, Qt::AlignCenter);
    setLayout(layout);
  }

  void setHovered(bool hovered) {
    m_hovered = hovered;
    update();
  }

  void setSelected(bool selected) {
    m_selected = selected;
    update();
  }
  void setInset(int inset) { layout->setContentsMargins(inset, inset, inset, inset); }
  void setIcon(const QString &name) { icon->setIcon(name, {56, 56}); }

signals:
  void clicked();
  void doubleClicked();
};

class GridItemWidget : public QWidget {
  Q_OBJECT

  QVBoxLayout *layout;
  EllidedLabel *titleLabel;
  EllidedLabel *subtitleLabel;
  bool m_selected;

public:
  MainWidget *main;

  GridItemWidget(QWidget *parent = nullptr)
      : QWidget(parent), layout(new QVBoxLayout), main(new MainWidget), titleLabel(new EllidedLabel),
        subtitleLabel(new EllidedLabel), m_selected(false) {
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(main);
    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    setLayout(layout);

    connect(main, &MainWidget::clicked, this, &GridItemWidget::clicked);
    connect(main, &MainWidget::doubleClicked, this, &GridItemWidget::doubleClicked);
  }

  void setInset(int inset) { main->setInset(inset); }

  void setSelected(bool selected) { main->setSelected(selected); }

  void setItem(const AbstractGridItem &item) {
    auto title = item.title();
    auto subtitle = item.subtitle();

    titleLabel->setText(title);
    subtitleLabel->setText(subtitle);
    titleLabel->setVisible(!title.isEmpty());
    subtitleLabel->setVisible(!subtitle.isEmpty());
    main->setIcon(item.iconName());
  }

signals:
  void clicked();
  void doubleClicked();
};

class VirtualGridWidget : public QWidget {
  Q_OBJECT

  int m_columns = 6;
  int m_inset = 0;
  int m_spacing = 10;
  int m_padding = 10;
  int m_selected = -1;

  QList<AbstractGridItem *> m_items;
  QWidget *viewport;
  QScrollBar *scrollBar;
  QList<QWidget *> visibleWidgets;

  void keyPressEvent(QKeyEvent *event) override {
    switch (event->key()) {
    case Qt::Key_Left:
      setSelected(qMax(0, m_selected - 1));
      break;
    case Qt::Key_Right:
      setSelected(qMin(m_selected + 1, m_items.size() - 1));
      break;
    case Qt::Key_Up:
      setSelected(qMax(0, m_selected - m_columns));
      break;
    case Qt::Key_Down:
      setSelected(qMin(m_selected + m_columns, m_items.size() - 1));
      break;
    case Qt::Key_Return:
      if (m_selected >= 0 && m_selected < m_items.size()) emit itemActivated(*m_items[m_selected]);
      break;
    }

    // QWidget::keyPressEvent(event);
  }

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    updateViewport();
  }

  void updateViewport() {
    int scrollHeight = scrollBar->value();
    int height = scrollHeight == 0 ? m_padding : 0;
    int width = m_padding;
    int totalSpacing = (m_columns - 1) * m_spacing;
    int columnWidth = (viewport->width() - totalSpacing - m_padding * 2) / m_columns;

    for (const auto &visible : visibleWidgets) {
      visible->deleteLater();
    }

    visibleWidgets.clear();

    int maxHeight = 0;

    for (int i = 0; i != m_items.size(); ++i) {
      auto item = m_items[i];
      auto widget = new GridItemWidget(viewport);

      connect(widget, &GridItemWidget::clicked, this, [this, i]() { setSelected(i); });
      connect(widget, &GridItemWidget::doubleClicked, this, [this, item]() { emit itemActivated(*item); });

      widget->setInset(m_inset);
      widget->setFixedWidth(columnWidth);
      widget->main->setFixedSize(columnWidth, columnWidth);
      widget->setSelected(m_selected == i);
      widget->setItem(*item);

      auto size = widget->sizeHint();

      widget->move(width, height);
      widget->show();

      qDebug() << "move" << width << height;

      visibleWidgets << widget;

      width += columnWidth + m_spacing;

      maxHeight = qMax(maxHeight, size.height());

      if (width + columnWidth > viewport->width()) {
        height += maxHeight + m_spacing;
        width = m_padding;
        maxHeight = 0;
      }
      if (height > viewport->height()) break;
    }
  }

public:
  void setColumns(int columns = 1) { this->m_columns = columns; }
  void setInset(int inset = 10) { this->m_inset = inset; }

  void setItems(const QList<AbstractGridItem *> &items) {
    m_items = items;
    m_selected = 0;

    GridItemWidget widget;

    for (int i = 0; i != items.size(); ++i) {
      auto item = items[i];

      widget.setItem(*item);
    }

    updateViewport();
  }

  void setSelected(int index) {
    if (m_selected == index || index < 0 || index >= m_items.size()) return;

    auto item = m_items[index];

    m_selected = index;
    updateViewport();
    emit selectionChanged(*item);
  }

  VirtualGridWidget() : viewport(new QWidget), scrollBar(new QScrollBar) {
    auto layout = new QHBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(viewport);
    // layout->addWidget(scrollBar);
    setLayout(layout);
  }

signals:
  void itemActivated(const AbstractGridItem &item);
  void selectionChanged(const AbstractGridItem &item);
};
