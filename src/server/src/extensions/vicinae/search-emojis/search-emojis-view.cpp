#include "search-emojis-view.hpp"
#include "clipboard-actions.hpp"
#include "common.hpp"
#include "emoji/emoji.hpp"
#include "extensions/vicinae/search-emojis/emoji-browser-model.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/emoji-service/emoji-service.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/action-pannel/push-action.hpp"
#include "ui/form/text-area.hpp"
#include "ui/views/form-view.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/views/typed-list-view.hpp"
#include "utils.hpp"

class EditEmojiKeywordsView : public ManagedFormView {
  TextArea *m_keywords = new TextArea;
  std::string_view m_emoji;

  void onSubmit() override {
    auto emojiService = context()->services->emojiService();
    auto toast = context()->services->toastService();

    if (emojiService->setCustomKeywords(m_emoji, m_keywords->text())) {
      toast->setToast("Keywords edited", ToastStyle::Success);
      popSelf();
    } else {
      toast->setToast("Failed to edit keywords", ToastStyle::Danger);
    }
  }

  void initializeForm() override {
    auto emojiService = context()->services->emojiService();
    auto metadata = emojiService->mapMetadata(m_emoji);

    m_keywords->setText(metadata.keywords);
    m_keywords->textEdit()->selectAll();
  }

public:
  EditEmojiKeywordsView(std::string_view emoji) : m_emoji(emoji) {
    auto inputField = new FormField();

    inputField->setWidget(m_keywords);
    inputField->setName("Keywords");
    inputField->setInfo("Additional keywords that will be used to index this emoji.");
    form()->addField(inputField);
  }
};

class ResetEmojiRankingAction : public AbstractAction {
  std::string_view m_emoji;

public:
  void execute(ApplicationContext *ctx) override {
    auto emojiService = ctx->services->emojiService();
    auto toast = ctx->services->toastService();

    if (emojiService->resetRanking(m_emoji)) {
      toast->setToast("Ranking successfuly reset");
    } else {
      toast->setToast("Failed to reset ranking", ToastStyle::Danger);
    }
  }
  QString title() const override { return "Reset ranking"; }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin("arrow-counter-clockwise"); }

  ResetEmojiRankingAction(std::string_view emoji) : m_emoji(emoji) {}
};

class VisitEmojiActionWrapper : public ProxyAction {
public:
  VisitEmojiActionWrapper(std::string_view emoji, AbstractAction *action)
      : ProxyAction(action), m_emoji(emoji) {}

  void executeAfter(ApplicationContext *ctx) override {
    auto emojiService = ctx->services->emojiService();
    emojiService->registerVisit(m_emoji);
  }

private:
  std::string_view m_emoji;
};

class PinEmojiAction : public AbstractAction {
  std::string_view m_emoji;

public:
  void execute(ApplicationContext *ctx) override {
    auto toast = ctx->services->toastService();

    if (ctx->services->emojiService()->pin(m_emoji)) {
      toast->setToast("Emoji pinned");
    } else {
      toast->setToast("Failed to pin emoji", ToastStyle::Danger);
    }
  }

  std::optional<ImageURL> icon() const override { return ImageURL::builtin("pin"); }
  QString title() const override { return "Pin emoji"; };

  PinEmojiAction(std::string_view emoji) : m_emoji(emoji) {}
};

class EditEmojiKeywordsAction : public PushAction<EditEmojiKeywordsView, std::string_view> {
  QString title() const override { return "Edit custom keywords"; }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin("text"); }

public:
  EditEmojiKeywordsAction(std::string_view emoji) : PushAction(emoji) {}
};

class UnpinEmojiAction : public AbstractAction {
  std::string_view m_emoji;

public:
  void execute(ApplicationContext *ctx) override {
    auto toast = ctx->services->toastService();
    auto emojiService = ctx->services->emojiService();

    if (emojiService->unpin(m_emoji)) {
      toast->setToast("Emoji unpinned");
    } else {
      toast->setToast("Failed to unpin emoji", ToastStyle::Danger);
    }
  }

  std::optional<ImageURL> icon() const override { return ImageURL::builtin("pin-disabled"); }
  QString title() const override { return "Unpin emoji"; };

  UnpinEmojiAction(std::string_view emoji) : m_emoji(emoji) {}
};

void EmojiView::regenerateMetaSections() {
  std::vector<const EmojiData *> pinned;
  std::vector<const EmojiData *> recent;

  for (const auto &visited : m_emojiService->getVisited()) {
    if (visited.pinnedAt) {
      pinned.emplace_back(visited.data);
    } else {
      recent.emplace_back(visited.data);
    }
  }

  m_model->setPinnedEmojis(pinned);
  m_model->setRecentEmojis(recent);
}

std::unique_ptr<ActionPanelState> EmojiView::createActionPanel(const ItemType &info) const {
  auto metadata = m_emojiService->mapMetadata(info.emoji);
  // the copied emoji should be toned, but the scoring should still target the generic version
  // so that it doesn't change on tone change.
  QString copiedEmoji = info.skinToneSupport && m_skinTone
                            ? emoji::applySkinTone(info.emoji, m_skinTone.value()).c_str()
                            : QString::fromUtf8(info.emoji);

  auto wm = context()->services->windowManager();
  auto panel = std::make_unique<ListActionPanelState>();
  auto copyEmoji = new CopyToClipboardAction(Clipboard::Text(copiedEmoji), "Copy emoji");
  auto copyName = new CopyToClipboardAction(
      Clipboard::Text(QString::fromUtf8(info.name.data(), info.name.size())), "Copy emoji name");
  auto copyGroup = new CopyToClipboardAction(
      Clipboard::Text(QString::fromUtf8(info.group.data(), info.group.size())), "Copy emoji group");
  auto editKeywords = new EditEmojiKeywordsAction(info.emoji);
  auto resetRanking = new ResetEmojiRankingAction(info.emoji);
  auto mainSection = panel->createSection();
  const QString defaultAction = command()->preferenceValues().value("defaultAction").toString();

  editKeywords->setShortcut(Keybind::EditAction);

  if (wm->canPaste()) {
    auto paste = new PasteToFocusedWindowAction(Clipboard::Text(copiedEmoji));

    if (defaultAction == "paste") {
      mainSection->addAction(new VisitEmojiActionWrapper(info.emoji, paste));
      mainSection->addAction(new VisitEmojiActionWrapper(info.emoji, copyEmoji));
    } else {
      mainSection->addAction(new VisitEmojiActionWrapper(info.emoji, copyEmoji));
      mainSection->addAction(new VisitEmojiActionWrapper(info.emoji, paste));
    }
  } else {
    mainSection->addAction(new VisitEmojiActionWrapper(info.emoji, copyEmoji));
  }

  mainSection->addAction(copyName);
  mainSection->addAction(copyGroup);
  mainSection->addAction(editKeywords);
  mainSection->addAction(resetRanking);

  if (metadata.pinnedAt) {
    mainSection->addAction(new UnpinEmojiAction(info.emoji));
  } else {
    mainSection->addAction(new PinEmojiAction(info.emoji));
  }

  return panel;
}

EmojiView::EmojiView() {
  auto emojiService = ServiceRegistry::instance()->emojiService();

  setHorizontalNavigation(true);
  connect(emojiService, &EmojiService::pinned, this, &EmojiView::handlePinned);
  connect(emojiService, &EmojiService::unpinned, this, &EmojiView::handleUnpinned);
  connect(emojiService, &EmojiService::visited, this, &EmojiView::handleVisited);
}

std::optional<QString> EmojiView::navigationTitle(const ItemType &item) const {
  return qStringFromStdView(item.name);
}

void EmojiView::textChanged(const QString &s) {

  if (s.isEmpty()) {
  } else {
    m_model->setSearchResults(m_emojiService->search(s.toStdString()));
  }

  m_model->setDisplayMode(s.isEmpty() ? EmojiBrowserModel::DisplayMode::Root
                                      : EmojiBrowserModel::DisplayMode::Search);
  m_model->requestUpdate();
  m_list->selectFirst();
}

void EmojiView::initialize() {
  TypedListView::initialize();
  m_list->setMargins(QMargins(20, 10, 20, 10));
  setSearchPlaceholderText("Search for emojis...");
  m_emojiService = context()->services->emojiService();
  m_model = new EmojiBrowserModel;
  regenerateMetaSections();
  m_model->setGroupedEmojis(m_emojiService->grouped());

  const auto skinToneId = command()->preferenceValues().value("skinTone").toString().toStdString();

  if (const auto it =
          std::ranges::find_if(emoji::skinTones(), [&](auto &&info) { return info.id == skinToneId; });
      it != emoji::skinTones().end()) {
    m_skinTone = it->tone;
  }

  setModel(m_model);
  m_model->setSkinTone(m_skinTone);
  textChanged(searchText());
}

void EmojiView::handlePinned(std::string_view emoji) {
  regenerateMetaSections();
  m_model->requestUpdate();
  m_list->selectFirst();
}

void EmojiView::handleUnpinned(std::string_view emoji) {
  regenerateMetaSections();
  m_model->requestUpdate();
  m_list->selectFirst();
}

void EmojiView::handleVisited(std::string_view emoji) {
  regenerateMetaSections();
  m_model->requestUpdate();
}
