#pragma once
#include "layout.hpp"

class SettingsItemInfo : public QWidget {
public:
  SettingsItemInfo(const std::vector<std::pair<QString, QString>> &metadata,
                   const std::optional<QString> &description) {
    VStack stack;

    if (description) {
      stack.add(
          VStack().add(UI::Text("Description").secondary()).addParagraph(description.value()).spacing(3));
    }

    for (const auto &pair : metadata) {
      stack.add(HStack()
                    .add(UI::Text(pair.first).secondary().fixed())
                    .addText(pair.second)
                    .spacing(10)
                    .justifyBetween());
    }

    stack.spacing(5).divided(1).margins(10).addStretch(1).imbue(this);
  }
};
