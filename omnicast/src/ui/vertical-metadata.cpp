#include "ui/vertical-metadata.hpp"
#include "common.hpp"
#include "tag.hpp"
#include "ui/text-label.hpp"
#include <qboxlayout.h>
#include <qlabel.h>

static QVBoxLayout *createContainer() {
  QVBoxLayout *layout = new QVBoxLayout;

  layout = new QVBoxLayout();
  layout->setAlignment(Qt::AlignTop);
  layout->setContentsMargins(12, 12, 12, 12);
  layout->setSpacing(15);

  return layout;
}

VerticalMetadata::VerticalMetadata() : layout(new QVBoxLayout) {
  layout->setAlignment(Qt::AlignTop);
  layout->setContentsMargins(0, 0, 0, 0);

  currentLayout = createContainer();
  layout->addLayout(currentLayout);

  setLayout(layout);
}

void VerticalMetadata::add(const QString &title, QWidget *widget) {
  auto row = new QWidget();
  auto rowLayout = new QVBoxLayout();
  auto titleWidget = new TextLabel(title);

  rowLayout->setContentsMargins(0, 0, 0, 0);
  rowLayout->setSpacing(10);
  rowLayout->addWidget(titleWidget, 0, Qt::AlignLeft);
  rowLayout->addWidget(widget, 0, Qt::AlignLeft);

  row->setLayout(rowLayout);
  currentLayout->addWidget(row);
}

void VerticalMetadata::addItem(const MetadataItem &item) {
  if (auto label = std::get_if<MetadataLabel>(&item)) {
    add(label->title, new QLabel(label->text));
  } else if (auto separator = std::get_if<MetadataSeparator>(&item)) {
    layout->addWidget(new HDivider);
    currentLayout = createContainer();
    layout->addLayout(currentLayout);
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
