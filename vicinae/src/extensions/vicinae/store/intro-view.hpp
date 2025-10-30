#pragma once
#include "extensions/vicinae/store/store-listing-view.hpp"
#include "layout.hpp"
#include "theme/colors.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/views/base-view.hpp"
#include "navigation-controller.hpp"
#include <qnamespace.h>

constexpr const char *description = R"(
# Welcome to the vicinae extension store

The Vicinae extension store lists extensions made by the community that have been approved by the core contributors of the Vicinae project. 

Every extension you will find in here has its source code available in the [vicinaehq/extensions](https://github.com/vicinaehq/extensions) repository. 

If you are looking to build your own extension, please go take a look at the [documentation](https://docs.vicinae.com/extensions/introduction). If you think your extension would fit in the store, feel free to submit it!
)";

// view shown the first time the user opens the store
class VicinaeStoreIntroView : public BaseView {
public:
  VicinaeStoreIntroView() {
    VStack()
        .add(UI::Icon(ImageURL::builtin("cart").setBackgroundTint(SemanticColor::Accent)).size({40, 40}), 0,
             Qt::AlignCenter)
        .markdown(description)
        .spacing(20)
        .margins(20)
        .addStretch()
        .imbue(this);
  }

  void initialize() override {
    auto next = new StaticAction("Continue to store", ImageURL::builtin("cart"), [this]() {
      command()->storage().setItem("introCompleted", true);
      qDebug() << "intro completed";
      QString id = command()->info().uniqueId();
      popSelf();
      context()->navigation->pushView(new VicinaeStoreListingView);
    });
    auto panel = std::make_unique<FormActionPanelState>();
    auto section = panel->createSection();

    section->addAction(next);
    setActions(std::move(panel));
    // add continue action
  }
};
