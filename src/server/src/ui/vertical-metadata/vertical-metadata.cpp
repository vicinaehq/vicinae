#include "ui/vertical-metadata/vertical-metadata.hpp"
#include "extend/tag-model.hpp"
#include "extensions/raycast/store/store-detail-view.hpp"
#include "theme.hpp"
#include "ui/divider/hdivider.hpp"
#include "utils/layout.hpp"
#include <qboxlayout.h>
#include <qlabel.h>
#include "ui/tag/tag.hpp"
#include <qwidget.h>

void VerticalMetadata::setMetadata(const std::vector<MetadataItem> &metadatas) {
  auto stack = VStack().spacing(10).margins(0, 10, 0, 0);

  int marginX = 10;

  for (const auto &metadata : metadatas) {
    if (auto link = std::get_if<MetadataLink>(&metadata)) {
      auto widget = new TextLinkWidget;

      widget->setText(link->text);
      widget->setHref(link->target);
      stack.add(VStack().marginsX(marginX).add(UI::Text(link->title).secondary()).add(widget).spacing(5));
    }

    if (auto label = std::get_if<MetadataLabel>(&metadata)) {
      auto textWidget = UI::Text(label->text);
      if (label->color) { textWidget.color(*label->color); }

      auto hstack = HStack()
                        .addIf(label->icon.has_value(),
                               [&]() -> QWidget * { return UI::Icon(*label->icon).size({16, 16}); })
                        .add(textWidget)
                        .spacing(5);

      stack.add(VStack().marginsX(marginX).add(UI::Text(label->title).secondary()).add(hstack).spacing(5));
    }

    if (auto tagList = std::get_if<TagListModel>(&metadata)) {
      auto hstack = Flow()
                        .map(tagList->items,
                             [](const TagItemModel &tag) {
                               auto widget = new TagWidget;

                               widget->setText(tag.text);
                               if (tag.color) widget->setColor(tag.color);
                               if (tag.icon) widget->setIcon(*tag.icon);

                               return widget;
                             })
                        .spacing(5);

      stack.add(VStack()
                    .marginsX(marginX)
                    .add(UI::Text(tagList->title).secondary())
                    .add(hstack.buildWidget())
                    .spacing(5));
    }

    if (auto sep = std::get_if<MetadataSeparator>(&metadata)) { stack.add(new HDivider); }
  }

  // Ensure remaining space is placed below content so items stay top-aligned
  stack.addStretch(1);

  stack.imbue(container);
}

VerticalMetadata::VerticalMetadata() : container(new QWidget) { setWidget(container); }
