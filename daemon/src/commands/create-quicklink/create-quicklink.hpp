#include "command-object.hpp"
#include "omnicast.hpp"
#include "ui/form_input.hpp"
#include "ui/input_field.hpp"
#include "ui/turbobox.hpp"
#include "xdg-desktop-database.hpp"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qnamespace.h>
#include <qwidget.h>

class CreateQuickLinkCommand : public CommandObject {
  Service<XdgDesktopDatabase> xdd;
  InputField *nameField;
  InputField *urlField;
  AppTurbobox *appField;

public:
  CreateQuickLinkCommand(AppWindow *app)
      : CommandObject(app), xdd(service<XdgDesktopDatabase>()) {
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

    // w->setStyleSheet("background-color: red");

    w->setLayout(layout);
    w->setFixedWidth(600);

    widget->setLayout(layout);
  }

  void onAttach() override {
    hideSearch();
    nameField->setFocus();
  }

  QIcon icon() override { return QIcon::fromTheme("link"); }
  QString name() override { return "Create quicklink"; }
};
