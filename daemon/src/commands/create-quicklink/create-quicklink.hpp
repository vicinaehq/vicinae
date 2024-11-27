#include "command-object.hpp"
#include "omnicast.hpp"
#include "ui/form_input.hpp"
#include "ui/turbobox.hpp"
#include "xdg-desktop-database.hpp"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qwidget.h>

class CreateQuickLinkCommand : public CommandObject {
  Service<XdgDesktopDatabase> xdd;
  FormInput *nameField;
  FormInput *urlField;
  FormInput *appField;

public:
  CreateQuickLinkCommand(AppWindow *app)
      : CommandObject(app), xdd(service<XdgDesktopDatabase>()) {
    auto layout = new QVBoxLayout();

    auto w = new QWidget();

    layout->setAlignment(Qt::AlignTop | Qt::AlignCenter);

    nameField = new FormInput("Name");
    urlField = new FormInput("URL");
    appField = new FormInput("App");

    nameField->setPlaceholder("Quicklink name");
    urlField->setPlaceholder("https://google.com/search?q={query}");
    appField->setPlaceholder("App to choose");

    layout->addWidget(nameField);
    layout->addWidget(urlField);
    layout->addWidget(this->appField);
    layout->addWidget(new AppTurbobox(xdd));

    // w->setStyleSheet("background-color: red");

    w->setLayout(layout);
    w->setFixedWidth(600);

    widget = new Container(w);

    for (const auto &app : xdd->query("")) {
      qDebug() << app.name;
    }
  }

  void onAttach() override {
    hideSearch();
    nameField->setFocus();
  }

  QIcon icon() override { return QIcon::fromTheme("link"); }
  QString name() override { return "Create quicklink"; }
};
