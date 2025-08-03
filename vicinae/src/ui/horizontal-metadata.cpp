#include "ui/horizontal-metadata.hpp"
#include "common.hpp"
#include "ui/tag/tag.hpp"
#include "utils/layout.hpp"
#include "ui/text-link/text-link.hpp"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qwidget.h>

void HorizontalMetadata::setMetadata(const std::vector<MetadataItem> &metadatas) {
  auto stack = VStack().spacing(10).margins(0, 0, 0, 0);

  int marginX = 10;

  for (const auto &metadata : metadatas) {
    if (auto link = std::get_if<MetadataLink>(&metadata)) {
      auto widget = new TextLinkWidget;

      widget->setText(link->text);
      widget->setHref(link->target);
      stack.add(HStack()
                    .marginsX(marginX)
                    .justifyBetween()
                    .add(UI::Text(link->title).secondary())
                    .add(widget)
                    .spacing(5));
    }

    if (auto label = std::get_if<MetadataLabel>(&metadata)) {
      auto hstack = HStack()
                        .addIf(label->icon.has_value(),
                               [&]() -> QWidget * { return UI::Icon(*label->icon).size({16, 16}); })
                        .add(UI::Text(label->text))
                        .spacing(5);

      stack.add(HStack()
                    .marginsX(marginX)
                    .justifyBetween()
                    .add(UI::Text(label->title).secondary())
                    .add(hstack)
                    .spacing(5));
    }

    if (auto tagList = std::get_if<TagListModel>(&metadata)) {
      auto hstack = HStack()
                        .map(tagList->items,
                             [](const TagItemModel &tag) {
                               auto widget = new TagWidget;

                               widget->setText(tag.text);
                               if (tag.icon) widget->setIcon(*tag.icon);
                               if (tag.color) widget->setColor(tag.color);

                               return widget;
                             })
                        .spacing(5);

      stack.add(HStack()
                    .marginsX(marginX)
                    .justifyBetween()
                    .add(UI::Text(tagList->title).secondary())
                    .add(hstack.buildWidget())
                    .spacing(5));
    }

    if (auto sep = std::get_if<MetadataSeparator>(&metadata)) { stack.add(new HDivider); }
  }

  stack.imbue(container);
}

HorizontalMetadata::HorizontalMetadata() : container(new QWidget) { setWidget(container); }
