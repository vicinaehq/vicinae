#pragma once
#include "common.hpp"
#include "omni-icon.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/omni-list.hpp"
#include "ui/selectable-omni-list-widget.hpp"
#include "ui/typography.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qmargins.h>
#include <qnamespace.h>
#include <qobjectdefs.h>
#include <qwidget.h>
#include <ranges>

class VirtualTreeItemDelegate {
  bool m_expanded = false;

public:
  virtual std::vector<VirtualTreeItemDelegate *> children() const { return {}; }

  virtual bool expandable() const { return false; }
  virtual QMargins contentMargins() const { return {5, 5, 5, 5}; }
  void setExpandable(bool value) { m_expanded = value; }
  bool expanded() const { return m_expanded; }
  virtual QWidget *widgetForColumn(int column) const { return nullptr; }
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
    m_columns = cols | std::views::transform([](auto &&str) { return ColumnInfo{.name = str}; }) |
                std::ranges::to<std::vector>();
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
    auto &theme = ThemeService::instance().theme();
    int borderWidth = 1;
    QPainter painter(this);

    QPen pen(theme.colors.border, borderWidth);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::transparent);
    painter.drawRect(rect());
  }

  void setupUI(HeaderInfo *info) {
    m_layout->setContentsMargins(5, 5, 5, 5);

    for (const auto [idx, col] : info->columns() | std::views::enumerate) {
      HeaderColumn *column = new HeaderColumn;

      if (idx > 0) { m_layout->addWidget(new VDivider); }

      if (col.sizePolicy == HeaderInfo::ColumnSizePolicy::Fixed) {
        column->setFixedWidth(col.width);
        qDebug() << "width" << col.width;
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
  Omnimg::ImageWidget *m_icon = new Omnimg::ImageWidget;

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
      m_icon->setUrl(BuiltinOmniIconUrl("chevron-down-small"));
    else
      m_icon->setUrl(BuiltinOmniIconUrl("chevron-right-small"));
  }

  LeftTableWidget() {
    m_icon->setUrl(BuiltinOmniIconUrl("chevron-right-small"));
    m_icon->setFixedSize(20, 20);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_icon);
    m_layout->addWidget(new QWidget, 1);
    setLayout(m_layout);
  }
};

class VirtualTreeItemRow : public OmniList::AbstractVirtualItem {
  VirtualTreeItemDelegate *m_delegate;
  HeaderInfo *m_header;
  int m_indent = 0;

public:
  void setIndentLevel(int indent) { m_indent = indent; }

  VirtualTreeItemDelegate *delegate() const { return m_delegate; }

  int computeLeftSpacing() const {
    int spacing = 0;

    if (!m_delegate->expandable()) spacing += 20;
    spacing += m_indent * 15;

    return spacing;
  }

  bool hasUniformHeight() const override { return true; }

  OmniListItemWidget *createWidget() const override {
    auto widget = new SelectableOmniListWidget;
    auto layout = new QHBoxLayout;

    layout->setContentsMargins(m_delegate->contentMargins());
    layout->addSpacing(computeLeftSpacing());

    auto cols = m_header->columns();

    for (int i = 0; i != cols.size(); ++i) {
      auto &column = cols.at(i);
      QWidget *widget = nullptr;
      int stretch = 1;

      if (i == 0) {
        auto left = new LeftTableWidget;
        QWidget *column = m_delegate->widgetForColumn(i);

        left->setFoldIconVisiblity(m_delegate->expandable());
        left->setFolded(m_delegate->expanded());
        left->setWidget(column);
        widget = left;
      } else {
        widget = m_delegate->widgetForColumn(i);
      }

      if (column.sizePolicy == HeaderInfo::ColumnSizePolicy::Fixed) {
        widget->setFixedWidth(column.width);
        stretch = 0;
      }

      layout->addWidget(widget, stretch);
    }

    widget->setLayout(layout);
    return widget;
  }

  VirtualTreeItemRow(VirtualTreeItemDelegate *delegate, HeaderInfo *header) {
    m_delegate = delegate;
    m_header = header;
  }
};

class OmniTree : public QWidget {
  OmniList *m_list = new OmniList;
  std::vector<VirtualTreeItemDelegate *> m_model;
  HeaderInfo m_header;
  QWidget *m_widget = new QWidget;
  QVBoxLayout *layout = new QVBoxLayout;

public:
  OmniTree(QWidget *parent = nullptr) : QWidget(parent) {

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_widget);
    layout->addWidget(m_list, 1);
    setLayout(layout);
    connect(m_list, &OmniList::itemActivated, this, &OmniTree::activateDelegate);
  }

  void setHeader(HeaderWidget *widget) {
    if (auto item = layout->itemAt(0)) {
      if (auto previous = item->widget()) {
        layout->replaceWidget(previous, widget);
        previous->deleteLater();
      }
    }
  }

  void setColumns(const std::vector<QString> &columns) {
    m_header.setColumns(columns);
    setHeader(new HeaderWidget(&m_header));
  }
  void setColumnSizePolicy(int idx, HeaderInfo::ColumnSizePolicy policy) {
    m_header.setColumnSizePolicy(idx, policy);
    setHeader(new HeaderWidget(&m_header));
  }
  void setColumnWidth(int index, int width) {
    m_header.setColumnWidth(index, width);
    setHeader(new HeaderWidget(&m_header));
  }

  void activateDelegate(const OmniList::AbstractVirtualItem &item) {
    auto &row = static_cast<const VirtualTreeItemRow &>(item);

    row.delegate()->setExpandable(!row.delegate()->expanded());
    renderModel();
  }

  void renderModel() {
    m_list->updateModel(
        [&]() {
          auto &section = m_list->addSection();

          for (const auto &row : m_model) {
            section.addItem(std::make_shared<VirtualTreeItemRow>(row, &m_header));

            if (row->expanded()) {
              for (const auto &row2 : row->children()) {
                auto item = std::make_shared<VirtualTreeItemRow>(row2, &m_header);

                item->setIndentLevel(1);
                section.addItem(item);
              }
            }
          }
        },
        OmniList::PreserveSelection);
  }

  void addRows(std::vector<VirtualTreeItemDelegate *> rows) {
    m_model = rows;
    renderModel();
  }
};
