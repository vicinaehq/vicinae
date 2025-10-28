#pragma once
#include "../image/url.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include "ui/image/image.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "ui/typography/typography.hpp"
#include <QtConcurrent/qtconcurrentiteratekernel.h>
#include <memory>
#include <qboxlayout.h>
#include <qevent.h>
#include <qgraphicseffect.h>
#include <qmargins.h>
#include <qnamespace.h>
#include <qobjectdefs.h>
#include <qstackedlayout.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class VirtualTreeItemDelegate {
  bool m_expanded = false;

public:
  virtual std::vector<std::shared_ptr<VirtualTreeItemDelegate>> children() const { return {}; }

  virtual QString id() const = 0;
  virtual bool expandable() const { return false; }
  virtual QMargins contentMargins() const { return {5, 5, 5, 5}; }
  void setExpandable(bool value) { m_expanded = value; }
  bool expanded() const { return m_expanded; }
  virtual QWidget *widgetForColumn(int column) const { return nullptr; }
  virtual void refreshForColumn(QWidget *widget, int column) const {}
  virtual bool disabled() const { return false; }
  virtual void attached(QWidget *widget, int column) {}
  virtual void detached(QWidget *widget, int column) {}
  virtual int colspan(int column) const { return 1; }
};

class HeaderInfo {
public:
  enum ColumnSizePolicy {
    Auto,
    Stretch,
    Fixed,
  };
  struct ColumnInfo {
    QString name;
    ColumnSizePolicy sizePolicy = ColumnSizePolicy::Auto;
    int width = 0;
  };

  std::vector<ColumnInfo> m_columns;

public:
  std::vector<ColumnInfo> columns() const { return m_columns; }
  void setColumns(const std::vector<QString> &cols) {
    m_columns.clear();
    for (const auto &col : cols) {
      m_columns.emplace_back(ColumnInfo{.name = col});
    }
  }
  void setColumns(const std::vector<ColumnInfo> &cols) { m_columns = cols; }
  void setColumnSizePolicy(int index, ColumnSizePolicy policy) {
    if (index < m_columns.size()) { m_columns.at(index).sizePolicy = policy; }
  }
  void setColumnWidth(int index, int width) {
    if (index < m_columns.size()) {
      auto &col = m_columns.at(index);
      col.sizePolicy = ColumnSizePolicy::Fixed;
      col.width = width;
    }
  }
};

class HeaderWidget : public QWidget {
  QHBoxLayout *m_layout = new QHBoxLayout;

  class HeaderColumn : public QWidget {
    TypographyWidget *m_typography = new TypographyWidget;

  public:
    void setText(const QString &text) { m_typography->setText(text); }
    HeaderColumn() {
      auto layout = new QHBoxLayout;
      layout->setContentsMargins(5, 0, 0, 0);
      layout->addWidget(m_typography);
      setLayout(layout);
    }
  };

  void paintEvent(QPaintEvent *event) override {
    int borderWidth = 1;
    OmniPainter painter(this);

    painter.setThemePen(SemanticColor::BackgroundBorder, borderWidth);
    painter.setThemeBrush(SemanticColor::ListItemHoverBackground);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawRect(rect());
  }

  void setupUI(HeaderInfo *info) {
    m_layout->setContentsMargins(5, 5, 5, 5);
    auto cols = info->columns();

    for (int i = 0; i != cols.size(); ++i) {
      auto &col = cols[i];
      HeaderColumn *column = new HeaderColumn;

      if (i > 0) { m_layout->addWidget(new VDivider); }

      if (col.sizePolicy == HeaderInfo::ColumnSizePolicy::Fixed) {
        column->setFixedWidth(col.width);
        m_layout->addWidget(column);
      } else {
        m_layout->addWidget(column, 1);
      }

      column->setText(col.name);
    }

    setLayout(m_layout);
  }

public:
  HeaderWidget(HeaderInfo *info) { setupUI(info); }
};

class LeftTableWidget : public QWidget {
  QHBoxLayout *m_layout = new QHBoxLayout;
  ImageWidget *m_icon = new ImageWidget;

public:
  void setWidget(QWidget *widget) {
    if (auto item = m_layout->itemAt(1)) {
      if (auto previous = item->widget()) {
        m_layout->replaceWidget(previous, widget);
        previous->deleteLater();
      }
    }
  }

  void setFoldIconVisiblity(bool value) { m_icon->setVisible(value); }
  void setFolded(bool value) {
    if (value)
      m_icon->setUrl(ImageURL::builtin("chevron-down-small"));
    else
      m_icon->setUrl(ImageURL::builtin("chevron-right-small"));
  }

  LeftTableWidget() {
    m_icon->setUrl(ImageURL::builtin("chevron-right-small"));
    m_icon->setFixedSize(20, 20);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_icon);
    m_layout->addWidget(new QWidget, 1);
    setLayout(m_layout);
  }
};

class OmniTreeActivatableWidget : public QWidget {
  Q_OBJECT

public:
  virtual void activated() {}
};

class OmniTreeRowWidget : public OmniListItemWidget {
  QGraphicsOpacityEffect *m_opacityEffect = new QGraphicsOpacityEffect(this);
  std::vector<QWidget *> m_columns;
  LeftTableWidget *m_left = new LeftTableWidget();
  QWidget *m_scene = new QWidget;
  QHBoxLayout *m_sceneLayout = new QHBoxLayout;
  int m_indent = 0;
  bool m_expandable = false;
  bool m_expanded = false;
  std::optional<ColorLike> m_color;
  bool m_selected = false;

  void selectionChanged(bool selected) override {
    m_selected = selected;
    update();
  }

  void activated() override {
    for (const auto &column : m_columns) {
      if (auto w = qobject_cast<OmniTreeActivatableWidget *>(column)) { w->activated(); }
    }
  }

  void paintEvent(QPaintEvent *event) override {
    OmniPainter painter(this);

    if (m_color || m_selected) {
      painter.setRenderHint(QPainter::Antialiasing);

      if (m_selected) {
        QColor color = painter.resolveColor(SemanticColor::Accent);
        color.setAlphaF(0.5);
        painter.setThemeBrush(color);
      } else {
        painter.setBrush(painter.colorBrush(*m_color));
      }

      painter.setPen(Qt::NoPen);
      painter.drawRoundedRect(rect(), 6, 6);
    }
  }

  void mouseDoubleClickEvent(QMouseEvent *event) override { emit doubleClicked(); }
  void mousePressEvent(QMouseEvent *event) override {
    if (event->button() == Qt::LeftButton) {
      emit clicked();
      return;
    }

    return OmniListItemWidget::mousePressEvent(event);
  }

public:
  void setBackgroundColor(const std::optional<ColorLike> &color) {
    m_color = color;
    update();
  }
  void setOpacity(qreal opacity) { m_opacityEffect->setOpacity(opacity); }
  void setIndent(int value) { m_indent = value; }
  void setExpanded(bool value) {
    m_expanded = value;
    m_left->setFoldIconVisiblity(value);
    m_left->setFolded(value);
  }
  void setExpandable(bool value) {
    m_expandable = value;
    m_left->setFoldIconVisiblity(value);
    m_left->setFolded(false);
  }

  auto widgets() const { return m_columns; }

  int computeLeftSpacing() const { return m_indent * 15 + ((!m_expandable && m_indent) * 20); }

  void setColumnWidgets(HeaderInfo *header, const std::vector<QWidget *> &widgets) {
    while (m_sceneLayout->count() > 0) {
      m_sceneLayout->takeAt(0);
    }

    m_sceneLayout->addSpacing(computeLeftSpacing());
    auto cols = header->columns();

    for (int i = 0; i != cols.size(); ++i) {
      auto &col = cols[i];
      QWidget *widget = nullptr;
      int stretch = 1;

      if (i == 0) {
        QWidget *column = widgets.at(i);

        m_left->setWidget(column);
        widget = m_left;
      } else {
        widget = widgets.at(i);
      }

      if (col.sizePolicy == HeaderInfo::ColumnSizePolicy::Fixed) {
        widget->setFixedWidth(col.width);
        stretch = 0;
      }
      m_sceneLayout->addWidget(widget, stretch);
    }

    for (const auto &widget : m_columns)
      widget->deleteLater();

    m_columns = widgets;
  }

public:
  OmniTreeRowWidget() {
    auto layout = new QStackedLayout;

    m_scene->setLayout(m_sceneLayout);
    m_scene->setGraphicsEffect(m_opacityEffect);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_scene);

    m_opacityEffect->setOpacity(1);
    setLayout(layout);
  }
};

class VirtualTreeItemRow : public OmniList::AbstractVirtualItem {
  VirtualTreeItemDelegate *m_delegate;
  HeaderInfo *m_header;
  std::optional<ColorLike> m_backgroundColor;
  int m_indent = 0;

  void forEachColumn(QWidget *widget, const std::function<void(QWidget *, size_t)> &fn) const {
    auto omniWidget = static_cast<OmniTreeRowWidget *>(widget);
    auto widgets = omniWidget->widgets();

    for (int i = 0; i != widgets.size(); ++i) {
      auto &widget = widgets[i];
      fn(widget, i);
    }
  }

public:
  void setIndentLevel(int indent) { m_indent = indent; }
  void setBackgroundColor(const std::optional<ColorLike> &color) { m_backgroundColor = color; }

  VirtualTreeItemDelegate *delegate() const { return m_delegate; }

  bool hasUniformHeight() const override { return true; }

  virtual void styleRow(OmniTreeRowWidget *widget) const {
    widget->setOpacity(m_delegate->disabled() ? 0.50 : 1);
    widget->setBackgroundColor(m_backgroundColor);
  }

  void refresh(QWidget *widget) const override {
    auto row = static_cast<OmniTreeRowWidget *>(widget);

    row->setExpanded(m_delegate->expanded());
    row->setIndent(m_indent);
    row->setExpandable(m_delegate->expandable());
    styleRow(row);
    forEachColumn(widget, [this](QWidget *widget, int idx) { m_delegate->refreshForColumn(widget, idx); });
  }

  void attached(QWidget *widget) override {
    forEachColumn(widget, [this](QWidget *widget, int idx) { m_delegate->attached(widget, idx); });
  }

  void detached(QWidget *widget) override {
    forEachColumn(widget, [this](QWidget *widget, int idx) { m_delegate->detached(widget, idx); });
  }

  OmniListItemWidget *createWidget() const override {
    auto widget = new OmniTreeRowWidget;

    styleRow(widget);

    widget->layout()->setContentsMargins(m_delegate->contentMargins());
    widget->setExpanded(m_delegate->expanded());
    widget->setIndent(m_indent);
    widget->setExpandable(m_delegate->expandable());

    auto cols = m_header->columns();
    std::vector<QWidget *> widgets;

    for (int i = 0; i != cols.size(); ++i) {
      auto &column = cols.at(i);
      auto widget = m_delegate->widgetForColumn(i);
      m_delegate->refreshForColumn(widget, i);
      widgets.emplace_back(widget);
    }

    widget->setColumnWidgets(m_header, widgets);

    return widget;
  }

  QString generateId() const override { return m_delegate->id(); }

  VirtualTreeItemRow(VirtualTreeItemDelegate *delegate, HeaderInfo *header) {
    m_delegate = delegate;
    m_header = header;
  }
};

/**
 * Wrapper around OmniList to render a tree-like list.
 * This is a bit hacky in its very essence but it gets the job done
 */
class OmniTree : public QWidget {
  Q_OBJECT

signals:
  void selectionUpdated(VirtualTreeItemDelegate *next, VirtualTreeItemDelegate *previous) const;

public:
  OmniTree(QWidget *parent = nullptr);

  bool selectUp() const;
  bool selectDown();
  bool selectHome() const;
  bool selectEnd();
  void activateCurrentSelection();
  bool selectFirst() const;
  bool select(const QString &id);
  VirtualTreeItemDelegate *value() const;
  void setHeader(HeaderWidget *widget);

  void setColumns(const std::vector<QString> &columns);
  void setColumnSizePolicy(int idx, HeaderInfo::ColumnSizePolicy policy);
  void setColumnWidth(int index, int width);
  void setAlternateBackgroundColor(const std::optional<ColorLike> &color);
  void activateDelegate(const OmniList::AbstractVirtualItem &item);
  void handleSelectionChanged(const OmniList::AbstractVirtualItem *next,
                              const OmniList::AbstractVirtualItem *previous);
  auto model() const;
  void refresh();
  VirtualTreeItemDelegate *itemAt(const QString &id);
  void renderModel(OmniList::SelectionPolicy policy = OmniList::PreserveSelection);
  void addRows(std::vector<std::shared_ptr<VirtualTreeItemDelegate>> rows);

private:
  OmniList *m_list = new OmniList;
  std::vector<std::shared_ptr<VirtualTreeItemDelegate>> m_model;
  HeaderInfo m_header;
  QWidget *m_widget = new QWidget;
  QVBoxLayout *layout = new QVBoxLayout;
  std::optional<ColorLike> m_alternateBackgroundColor;
};
