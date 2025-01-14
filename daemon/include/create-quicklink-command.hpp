#pragma once
#include "app-database.hpp"
#include "app.hpp"
#include "ui/action_popover.hpp"
#include "ui/form.hpp"
#include "view.hpp"
#include <functional>
#include <memory>
#include <qnamespace.h>

class CallbackAction : public AbstractAction {
  using SubmitHandler = std::function<void(AppWindow &app)>;
  SubmitHandler handler;

public:
  CallbackAction(const SubmitHandler &localHandler)
      : AbstractAction("Submit", {.iconName = "submit"}), handler(localHandler) {}

  void execute(AppWindow &app) override { handler(app); }
};

class AppSelectorItem : public AbstractFormDropdownItem {
public:
  std::shared_ptr<DesktopExecutable> app;

  QIcon icon() const override { return app->icon(); }
  QString displayName() const override { return app->name; }

  AppSelectorItem(const std::shared_ptr<DesktopExecutable> &app) : app(app) {}
};

class CreateQuicklinkCommandView : public View {
  Service<AppDatabase> appDb;
  FormWidget *form;
  FormInputWidget *name;
  FormInputWidget *link;
  FormDropdown *appSelector;

  void handleAppSelectorTextChanged(const QString &text) {
    appSelector->model()->beginReset();
    for (const auto &app : appDb.apps) {
      if (!app->name.contains(text, Qt::CaseInsensitive)) continue;

      appSelector->model()->addItem(std::make_shared<AppSelectorItem>(app));

      qDebug() << "app selector changed";
    }
    appSelector->model()->endReset();
  }

public:
  CreateQuicklinkCommandView(AppWindow &app)
      : View(app), appDb(service<AppDatabase>()), form(new FormWidget), name(new FormInputWidget("name")),
        link(new FormInputWidget("link")), appSelector(new FormDropdown) {
    name->setName("Name");
    form->addInput(name);
    link->setName("URL");
    form->addInput(link);
    form->addInput(appSelector);

    connect(appSelector, &FormDropdown::textChanged, this,
            &CreateQuicklinkCommandView::handleAppSelectorTextChanged);

    widget = form;
  }

  void onMount() override {
    auto submitAction = new CallbackAction([this](AppWindow &app) { submit(app); });

    submitAction->setShortcut(KeyboardShortcutModel{.key = "return", .modifiers = {"ctrl"}});

    setSignalActions({submitAction});
  }

  void submit(AppWindow &app) {
    qDebug() << "creating link" << name->text() << "for url" << link->text() << "description";
    auto item = std::static_pointer_cast<AppSelectorItem>(appSelector->value());

    if (!item) {
      qDebug() << "no app selected";
      return;
    }

    qDebug() << "app" << item->app->id;

    app.statusBar->setToast("Submitted");
  }
};
