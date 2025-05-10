#include "ui/metadata-pane.hpp"
#include "common.hpp"
#include "tag.hpp"
#include "theme.hpp"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <variant>

MetadataPane::MetadataPane(Direction direction) : layout(new QVBoxLayout), direction(direction) {
  layout->setAlignment(Qt::AlignTop);

  if (direction == Vertical) {
    layout->setContentsMargins(0, 0, 0, 0);
  } else {
    layout->setContentsMargins(10, 10, 10, 10);
  }

  setLayout(layout);
}

void MetadataPane::add(const QString &title, QWidget *widget, Direction direction) {
  auto row = new QWidget();
  QBoxLayout *rowLayout = nullptr;

  if (direction == Vertical)
    rowLayout = new QVBoxLayout;
  else
    rowLayout = new QHBoxLayout;

  auto titleWidget = new QLabel(title);

  titleWidget->setStyleSheet("QLabel { font-weight: 600; opacity: 0.2; }");
  rowLayout->setSpacing(10);

  if (direction == Vertical) {
    rowLayout->setContentsMargins(15, 10, 15, 10);
    rowLayout->addWidget(titleWidget, 0, Qt::AlignLeft);
    rowLayout->addWidget(widget, 0, Qt::AlignLeft);
  } else {
    rowLayout->setContentsMargins(0, 0, 0, 0);
    rowLayout->addWidget(titleWidget, 0, Qt::AlignLeft | Qt::AlignVCenter);
    rowLayout->addWidget(widget, 0, Qt::AlignRight | Qt::AlignVCenter);
  }

  row->setLayout(rowLayout);
  layout->addWidget(row);
}

void MetadataPane::addItem(const MetadataItem &item, Direction direction) {
  if (auto label = std::get_if<MetadataLabel>(&item)) {
    add(label->title, new QLabel(label->text), direction);
  } else if (auto separator = std::get_if<MetadataSeparator>(&item)) {
    layout->addWidget(new HDivider);
  } else if (auto tagList = std::get_if<TagListModel>(&item)) {
    auto widget = new TagList;

    for (const auto &model : tagList->items) {
      auto tag = new Tag();

      tag->applyModel(model);
      widget->addTag(tag);
    }

    add(tagList->title, widget, direction);
  }
}
