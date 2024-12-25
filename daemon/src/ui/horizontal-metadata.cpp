#include "ui/horizontal-metadata.hpp"
#include "common.hpp"
#include "tag.hpp"
#include "theme.hpp"
#include "ui/text-label.hpp"
#include <qboxlayout.h>
#include <qlabel.h>

HorizontalMetadata::HorizontalMetadata() : layout(new QVBoxLayout) {
  layout->setAlignment(Qt::AlignTop);
  layout->setContentsMargins(12, 12, 12, 12);
  layout->setSpacing(12);

  setLayout(layout);
}

void HorizontalMetadata::add(const QString &title, QWidget *widget) {
  auto row = new QWidget();
  auto rowLayout = new QHBoxLayout();
  auto titleWidget = new TextLabel(title);

  rowLayout->setContentsMargins(0, 0, 0, 0);
  rowLayout->addWidget(titleWidget, 0, Qt::AlignLeft | Qt::AlignVCenter);
  rowLayout->addWidget(widget, 0, Qt::AlignRight | Qt::AlignVCenter);

  row->setLayout(rowLayout);
  layout->addWidget(row);
}

void HorizontalMetadata::addItem(const MetadataItem &item) {
  ThemeService theme;

  if (auto label = std::get_if<MetadataLabel>(&item)) {
    add(label->title, new QLabel(label->text));
  } else if (auto separator = std::get_if<MetadataSeparator>(&item)) {
    layout->addWidget(new HDivider);
  } else if (auto tagList = std::get_if<TagListModel>(&item)) {
    auto widget = new TagList;

    for (const auto &model : tagList->items) {
      auto tag = new Tag();

      tag->applyModel(model);
      widget->addTag(tag);
    }

    add(tagList->title, widget);
  }
}
