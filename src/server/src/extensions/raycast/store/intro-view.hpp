#pragma once
#include "extensions/raycast/store/store-listing-view.hpp"
#include "layout.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/image/url.hpp"
#include "ui/markdown/markdown-renderer.hpp"
#include "ui/views/base-view.hpp"
#include "navigation-controller.hpp"
#include <qnamespace.h>

static const QString INTRO = R"(
# Welcome to the Raycast Extension Store

Vicinae provides direct integration with the official [Raycast store](https://www.raycast.com/store), allowing you to search and install Raycast extensions directly from Vicinae.

Please note that many extensions may not fully work at this time. This is either due to missing feature implementations in Vicinae, or extensions relying on macOS-specific tools and APIs.

Vicinae also has its own [extension store](vicinae://extensions/vicinae/vicinae/store), which does not suffer from these limitations.
)";

// view shown the first time the user opens the store
class RaycastStoreIntroView : public BaseView {
public:
  RaycastStoreIntroView() {}

  ImageURL icon() { return command()->info().iconUrl(); }
  bool supportsSearch() const override { return false; };

  void setupUI() {
    VStack()
        .addIcon(icon(), {40, 40}, Qt::AlignCenter)
        .add(m_markdown, 1)
        .spacing(20)
        .margins(20)
        .addStretch()
        .imbue(this);
    m_markdown->setMarkdown(INTRO);
  }

  void initialize() override {
    setupUI();
    auto next = new StaticAction("Continue to store", icon(), [this]() {
      command()->storage().setItem("introCompleted", true);
      context()->navigation->replaceView<RaycastStoreListingView>();
    });
    auto panel = std::make_unique<FormActionPanelState>();
    auto section = panel->createSection();
    section->addAction(next);
    setActions(std::move(panel));
  }

private:
  MarkdownRenderer *m_markdown = new MarkdownRenderer;
};
