#include "command-object.hpp"
#include "omnicast.hpp"
#include <qboxlayout.h>
#include <qlabel.h>

class CreateQuickLinkCommand : public CommandObject {
public:
  CreateQuickLinkCommand(AppWindow *app) : CommandObject(app) {
    auto layout = new QHBoxLayout();

    layout->addWidget(new QLabel("Hello"));

    widget->setLayout(layout);
  }

  void onAttach() override { hideSearch(); }

  QIcon icon() override { return QIcon::fromTheme("link"); }
  QString name() override { return "Create quicklink"; }
};
