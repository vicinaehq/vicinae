#pragma once
#include "emoji/emoji.hpp"
#include "services/emoji-service/emoji-service.hpp"
#include "ui/views/typed-list-view.hpp"
#include "emoji-browser-model.hpp"

class EmojiView : public TypedListView<EmojiBrowserModel> {
public:
  EmojiView();
  void initialize() override;
  void handlePinned(std::string_view emoji);
  void handleUnpinned(std::string_view emoji);
  void handleVisited(std::string_view emoji);
  void textChanged(const QString &s) override;

protected:
  void regenerateMetaSections();

  std::unique_ptr<ActionPanelState> createActionPanel(const ItemType &item) const override;
  std::optional<QString> navigationTitle(const ItemType &item) const override;

  EmojiService *m_emojiService = nullptr;
  EmojiBrowserModel *m_model = nullptr;
  std::optional<emoji::SkinTone> m_skinTone;
};
