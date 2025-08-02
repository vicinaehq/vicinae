#include "ui/horizontal-metadata.hpp"
#include "common.hpp"
#include "ui/tag/tag.hpp"
#include "ui/typography/typography.hpp"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qwidget.h>

HorizontalMetadata::HorizontalMetadata() : layout(new QVBoxLayout) {
  auto widget = new QWidget;

  layout->setContentsMargins(12, 12, 12, 12);
  layout->setSpacing(10);
  layout->addStretch();
  widget->setLayout(layout);
  setWidget(widget);
}

void HorizontalMetadata::add(const QString &title, QWidget *widget) {
  auto row = new QWidget();
  auto rowLayout = new QHBoxLayout();
  auto titleWidget = new TypographyWidget;

  titleWidget->setText(title);
  titleWidget->setColor(SemanticColor::TextSecondary);
  // I'm not sure why we need this, but otherwise the label can end up being cut off.
  titleWidget->setFixedWidth(titleWidget->sizeHint().width());

  rowLayout->setContentsMargins(0, 0, 0, 0);
  rowLayout->setSpacing(0);
  rowLayout->addWidget(titleWidget);
  rowLayout->addStretch();
  rowLayout->addWidget(widget);

  row->setLayout(rowLayout);
  layout->addWidget(row);
}

void HorizontalMetadata::addItem(const MetadataItem &item) {
  if (auto label = std::get_if<MetadataLabel>(&item)) {
    auto labelWidget = new TypographyWidget();

    labelWidget->setText(label->text);
    labelWidget->setEllideMode(Qt::TextElideMode::ElideMiddle);
    labelWidget->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    // labelWidget->setAutoEllide(false);

    add(label->title, labelWidget);
  } else if (auto separator = std::get_if<MetadataSeparator>(&item)) {
    layout->addWidget(new HDivider);
  } else if (auto tagList = std::get_if<TagListModel>(&item)) {
    /*
auto widget = new TagList;

for (const auto &model : tagList->items) {
auto tag = new Tag();

tag->applyModel(model);
widget->addTag(tag);
}

add(tagList->title, widget);
  */
  }
}

void HorizontalMetadata::clear() {
  while (auto item = layout->takeAt(0)) {
    if (auto widget = item->widget()) widget->deleteLater();
  }
}
