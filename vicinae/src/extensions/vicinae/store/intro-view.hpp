#pragma once
#include "extensions/vicinae/store/store-listing-view.hpp"
#include "layout.hpp"
#include "theme/colors.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/markdown/markdown-renderer.hpp"
#include "ui/views/base-view.hpp"
#include "navigation-controller.hpp"
#include <qnamespace.h>

const QString INTRO = R"(
# Welcome to the Vicinae Extension Store

The Vicinae extension store features community-built extensions that have been approved by the core contributors of the Vicinae project.

Every extension listed here has its source code available in the [vicinaehq/extensions](https://github.com/vicinaehq/extensions) repository.

If you're looking to build your own extension, take a look at the [documentation](https://docs.vicinae.com/extensions/introduction). If you think your extension would be a good fit for the store, feel free to submit it!
)";

// view shown the first time the user opens the store
class VicinaeStoreIntroView : public BaseView {
public:
  VicinaeStoreIntroView() {
    VStack()
        .add(UI::Icon(ImageURL::builtin("cart").setBackgroundTint(SemanticColor::Accent)).size({40, 40}), 0,
             Qt::AlignCenter)
        .add(m_markdown, 1)
        .spacing(20)
        .margins(20)
        .addStretch()
        .imbue(this);
    m_markdown->setMarkdown(INTRO);
  }

  bool supportsSearch() const override { return false; }

  void initialize() override {
    auto next = new StaticAction("Continue to store", ImageURL::builtin("cart"), [this]() {
      command()->storage().setItem("introCompleted", true);
      context()->navigation->replaceView<VicinaeStoreListingView>();
    });
    auto panel = std::make_unique<FormActionPanelState>();
    auto section = panel->createSection();

    m_markdown->textEdit()->setFocus();
    section->addAction(next);
    setActions(std::move(panel));
  }

private:
  MarkdownRenderer *m_markdown = new MarkdownRenderer;
};
