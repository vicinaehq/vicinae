#include "command-object.hpp"
#include "common.hpp"
#include "omnicast.hpp"
#include "quicklist-database.hpp"
#include "ui/form_input.hpp"
#include "ui/input_field.hpp"
#include "ui/turbobox.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qwidget.h>

class Submission : public IAction {
public:
  QString name() const override { return "Create link"; }
  QIcon icon() const override { return QIcon::fromTheme("link"); }
};

class CreateQuickLinkCommand : public CommandObject {
  Service<QuicklistDatabase> linkDb;
  Service<AppDatabase> xdd;
  InputField *nameField;
  InputField *urlField;
  AppTurbobox *appField;

public:
  CreateQuickLinkCommand(AppWindow *app)
      : CommandObject(app), linkDb(service<QuicklistDatabase>()),
        xdd(service<AppDatabase>()) {
    auto layout = new QVBoxLayout();

    auto w = new QWidget();

    layout->setAlignment(Qt::AlignTop | Qt::AlignCenter);

    nameField = new InputField();
    urlField = new InputField();
    appField = new AppTurbobox(xdd);

    nameField->setPlaceholderText("Quicklink name");
    urlField->setPlaceholderText("https://google.com/search?q={query}");

    layout->addWidget(new FormInput("Name", nameField));
    layout->addWidget(new FormInput("URL", urlField));
    layout->addWidget(new FormInput("App", appField));

    w->setLayout(layout);
    w->setFixedWidth(600);

    if (auto browser = xdd->defaultBrowser()) {
      appField->setSelected(browser);
    }

    widget->setLayout(layout);

    setActions({std::make_shared<Submission>()});
  }

  void onActionActivated(std::shared_ptr<IAction> action) override {
    linkDb->addLink({.name = nameField->text(),
                     .icon = appField->selected->icon().name(),
                     .link = urlField->text(),
                     .app = appField->selected->id});
    popCurrent();
  }

  void onAttach() override {
    hideSearch();
    nameField->setFocus();
  }

  QIcon icon() override { return QIcon::fromTheme("link"); }
  QString name() override { return "Create quicklink"; }
};
