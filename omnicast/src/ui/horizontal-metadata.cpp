#include "ui/horizontal-metadata.hpp"
#include "common.hpp"
#include "tag.hpp"
#include "ui/typography/typography.hpp"
#include <qboxlayout.h>
#include <qlabel.h>

HorizontalMetadata::HorizontalMetadata() : layout(new QVBoxLayout) {
  layout->setAlignment(Qt::AlignTop);
  layout->setContentsMargins(12, 12, 12, 12);
  layout->setSpacing(10);

  setLayout(layout);
}

void HorizontalMetadata::add(const QString &title, QWidget *widget) {
  auto row = new QWidget();
  auto rowLayout = new QHBoxLayout();
  auto titleWidget = new TypographyWidget;

  titleWidget->setText(title);
  titleWidget->setColor(ColorTint::TextSecondary);

  rowLayout->setContentsMargins(0, 0, 0, 0);
  rowLayout->addWidget(titleWidget, 0, Qt::AlignLeft | Qt::AlignVCenter);
  rowLayout->addWidget(widget, 0, Qt::AlignRight | Qt::AlignVCenter);

  row->setLayout(rowLayout);
  layout->addWidget(row);
}

void HorizontalMetadata::addItem(const MetadataItem &item) {
  if (auto label = std::get_if<MetadataLabel>(&item)) {
    auto labelWidget = new TypographyWidget();

    labelWidget->setText(label->text);

    add(label->title, labelWidget);
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

void HorizontalMetadata::clear() {
  while (auto item = layout->takeAt(0)) {
    if (auto widget = item->widget()) widget->deleteLater();
  }
}
