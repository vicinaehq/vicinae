#pragma once
#include "navigation-controller.hpp"
#include "omni-icon.hpp"
#include "simple-view.hpp"
#include "ui/action-pannel/action.hpp"
#include <qlogging.h>

class FormView : public SimpleView {
public:
  virtual void onSubmit() = 0;

  /**
   * Customize submit action title
   */
  virtual QString submitTitle() const { return "Submit"; }

  void initialize() override final {
    initializeForm();

    auto panel = std::make_unique<ActionPanelState>();
    auto section = panel->createSection();
    auto submit = new StaticAction(submitTitle(), BuiltinOmniIconUrl("enter-key"), [this]() { onSubmit(); });

    submit->setPrimary(true);
    submit->setShortcut({.key = "return", .modifiers = {"shift"}});
    section->addAction(submit);
    setActions(std::move(panel));
  }

  virtual void initializeForm() {}

  FormView(QWidget *parent = nullptr) : SimpleView(parent) {}

  bool needsGlobalStatusBar() const override { return true; }
  bool supportsSearch() const override { return false; }
};
