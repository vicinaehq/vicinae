#include "ui/horizontal-metadata.hpp"
#include "ui/divider/hdivider.hpp"
#include "ui/tag/tag.hpp"
#include "utils/layout.hpp"
#include "ui/text-link/text-link.hpp"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qwidget.h>

void HorizontalMetadata::setMetadata(const std::vector<MetadataItem> &metadatas) {
  auto stack = VStack().spacing(10).marginsY(10);

  int marginX = 10;
  int minHorizontalSpacing = 10;

  for (const auto &metadata : metadatas) {
    if (auto link = std::get_if<MetadataLink>(&metadata)) {
      auto widget = new TextLinkWidget;

      widget->setText(link->text);
      widget->setHref(link->target);
      stack.add(HStack()
                    .marginsX(marginX)
                    .justifyBetween()
                    .add(UI::Text(link->title).secondary().fixed())
                    .add(widget)
                    .spacing(minHorizontalSpacing));
    }

    if (auto label = std::get_if<MetadataLabel>(&metadata)) {
      auto textWidget = UI::Text(label->text);
      if (label->color) { textWidget.color(*label->color); }

      auto hstack = HStack()
                        .addIf(label->icon.has_value(),
                               [&]() -> QWidget * { return UI::Icon(*label->icon).size({16, 16}); })
                        .add(textWidget)
                        .spacing(5);

      stack.add(HStack()
                    .marginsX(marginX)
                    .justifyBetween()
                    .add(UI::Text(label->title).secondary().fixed())
                    .add(hstack)
                    .spacing(minHorizontalSpacing));
    }

    if (auto tagList = std::get_if<TagListModel>(&metadata)) {
      auto hstack = HStack()
                        .map(tagList->items,
                             [](const TagItemModel &tag) {
                               auto widget = new TagWidget;

                               widget->setText(tag.text);
                               if (tag.color) widget->setColor(tag.color);
                               if (tag.icon) widget->setIcon(*tag.icon);

                               return widget;
                             })
                        .spacing(5)
                        .buildWidget();

      stack.add(HStack()
                    .marginsX(marginX)
                    .add(UI::Text(tagList->title).fixed().secondary())
                    .add(hstack)
                    .justifyBetween()
                    .spacing(minHorizontalSpacing));
    }

    if (auto sep = std::get_if<MetadataSeparator>(&metadata)) { stack.add(new HDivider); }
  }

  // stack.addStretch();
  stack.imbue(container);
}

HorizontalMetadata::HorizontalMetadata() : container(new QWidget) { setWidget(container); }
