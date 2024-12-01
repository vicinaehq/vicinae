#include "command-database.hpp"

CreateQuickLinkCommand::CreateQuickLinkCommand(AppWindow *app)
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

void CreateQuickLinkCommand::onActionActivated(
    std::shared_ptr<IAction> action) {
  linkDb->insertLink({.name = nameField->text(),
                      .icon = appField->selected->icon().name(),
                      .link = urlField->text(),
                      .app = appField->selected->id});
  popCurrent();
}

void CreateQuickLinkCommand::onAttach() {
  hideSearch();
  nameField->setFocus();
}

QIcon CreateQuickLinkCommand::icon() { return QIcon::fromTheme("link"); }
QString CreateQuickLinkCommand::name() { return "Create quicklink"; }
