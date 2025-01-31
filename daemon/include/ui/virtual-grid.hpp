#pragma once
#include "omni-icon.hpp"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qscrollbar.h>
#include <qwidget.h>

class AbstractGridItem {
public:
  virtual QString title() const { return {}; }
  virtual QString subtitle() const { return {}; }
  virtual QString iconName() const = 0;
};

class GridItemWidget : public QWidget {
  class MainWidget : public QWidget {
    QVBoxLayout *layout;
    OmniIcon *icon;

    void paintEvent(QPaintEvent *event) override {
      int borderRadius = 10;
      QColor borderColor("#202020");

      QPainter painter(this);

      painter.setRenderHint(QPainter::Antialiasing, true);

      QPainterPath path;
      path.addRoundedRect(rect(), borderRadius, borderRadius);

      painter.setClipPath(path);

      QColor backgroundColor("#202020");

      painter.fillPath(path, backgroundColor);

      QPen pen(borderColor, 1); // Border with a thickness of 2
      painter.setPen(pen);
      painter.drawPath(path);
    }

  public:
    MainWidget() : layout(new QVBoxLayout), icon(new OmniIcon) {
      layout->addWidget(icon, 0, Qt::AlignCenter);
      setLayout(layout);
    }

    void setIcon(const QString &name) { icon->setIcon(name, {64, 64}); }
  };

  QVBoxLayout *layout;
  QLabel *title;
  QLabel *subtitle;

public:
  MainWidget *main;

  GridItemWidget(QWidget *parent = nullptr)
      : QWidget(parent), layout(new QVBoxLayout), main(new MainWidget), title(new QLabel),
        subtitle(new QLabel) {
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(main);
    layout->addWidget(title);
    layout->addWidget(subtitle);
    setLayout(layout);
  }

  void setItem(const AbstractGridItem &item) {
    title->setText(item.title());
    subtitle->setText(item.subtitle());
    main->setIcon(item.iconName());
  }
};

class VirtualGridWidget : public QWidget {
  int m_columns = 6;
  int m_inset = 0;
  int m_spacing = 10;
  int m_padding = 10;

  QList<AbstractGridItem *> m_items;
  QWidget *viewport;
  QScrollBar *scrollBar;
  QList<QWidget *> visibleWidgets;

  void updateViewport() {
    int height = 0;
    int width = m_padding;
    int totalSpacing = (m_columns - 1) * m_spacing;
    int columnWidth = (viewport->width() - totalSpacing - m_padding * 2) / m_columns;

    for (const auto &visible : visibleWidgets) {
      visible->deleteLater();
    }

    visibleWidgets.clear();

    int maxHeight = 0;

    for (auto item : m_items) {
      auto widget = new GridItemWidget(viewport);

      widget->main->setFixedSize(columnWidth, columnWidth);
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
    updateViewport();
  }

  VirtualGridWidget() : viewport(new QWidget), scrollBar(new QScrollBar) {
    auto layout = new QHBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(viewport);
    // layout->addWidget(scrollBar);
    setLayout(layout);
  }
};
