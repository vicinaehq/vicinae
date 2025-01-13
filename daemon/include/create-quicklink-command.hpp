#pragma once
#include "app.hpp"
#include "ui/action_popover.hpp"
#include "ui/form.hpp"
#include "view.hpp"
#include <functional>

class CallbackAction : public AbstractAction {
  using SubmitHandler = std::function<void(AppWindow &app)>;
  SubmitHandler handler;

public:
  CallbackAction(const SubmitHandler &localHandler)
      : AbstractAction("Submit", {.iconName = "submit"}), handler(localHandler) {}

  void execute(AppWindow &app) override { handler(app); }
};

class CreateQuicklinkCommandView : public View {
  FormWidget *form;
  FormInputWidget *name;
  FormInputWidget *link;
  FormInputWidget *description;

public:
  CreateQuicklinkCommandView(AppWindow &app)
      : View(app), form(new FormWidget), name(new FormInputWidget("name")), link(new FormInputWidget("link")),
        description(new FormInputWidget("description")) {
    form->addInput(name);
    form->addInput(link);
    form->addInput(description);
    widget = form;
  }

  void onMount() override {
    auto submitAction = new CallbackAction([this](AppWindow &app) { submit(app); });

    submitAction->setShortcut(KeyboardShortcutModel{.key = "return", .modifiers = {"ctrl"}});

    setSignalActions({submitAction});
  }

  void submit(AppWindow &app) {
    qDebug() << "creating link" << name->text() << "for url" << link->text() << "description"
             << description->text();
    app.statusBar->setToast("Submitted");
  }
};
