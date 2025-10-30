#pragma once
#include "extensions/raycast/store/store-listing-view.hpp"
#include "layout.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/image/url.hpp"
#include "ui/views/base-view.hpp"
#include "navigation-controller.hpp"
#include <qnamespace.h>

constexpr const char *description = R"(
# Welcome to the Raycast extension store

Vicinae provides a direct integration with the official [Raycast store](https://www.raycast.com/store), allowing you to search and install existing Raycast extensions from
Vicinae directly.

Please note that at the moment, a lot of extensions may not fully work, due to us either not implementing some features, or the extension relying on MacOS specific tools or APIs.
)";

// view shown the first time the user opens the store
class RaycastStoreIntroView : public BaseView {
public:
  RaycastStoreIntroView() {}

  ImageURL icon() { return command()->info().iconUrl(); }

  void setupUI() {
    VStack()
        .addIcon(icon(), {40, 40}, Qt::AlignCenter)
        .markdown(description)
        .spacing(20)
        .margins(20)
        .addStretch()
        .imbue(this);
  }

  void initialize() override {
    setupUI();
    auto next = new StaticAction("Continue to store", icon(), [this]() {
      command()->storage().setItem("introCompleted", true);
      qDebug() << "intro completed";
      QString id = command()->info().uniqueId();
      popSelf();
      context()->navigation->pushView(new RaycastStoreListingView);
    });
    auto panel = std::make_unique<FormActionPanelState>();
    auto section = panel->createSection();

    section->addAction(next);
    setActions(std::move(panel));
  }
};
