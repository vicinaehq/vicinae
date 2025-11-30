#pragma once
#include "services/emoji-service/emoji.hpp"
#include "ui/views/grid-view.hpp"
#include "ui/vlist/common/simple-grid-model.hpp"

class EmojiView : public GridView {
public:
  EmojiView();
  void initialize() override;
  void resetList();
  void handlePinned(std::string_view emoji);
  void handleUnpinned(std::string_view emoji);
  void handleVisited(std::string_view emoji);
  void generateRootList(OmniList::SelectionPolicy selectionPolicy = OmniList::SelectFirst);
  void generateFilteredList(const QString &s);
  void textChanged(const QString &s) override;
};
