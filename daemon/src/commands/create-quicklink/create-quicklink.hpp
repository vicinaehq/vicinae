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
  void exec(ExecutionContext ctx) override {}
};

class CreateQuickLinkCommand : public CommandObject {
  Service<QuicklistDatabase> linkDb;
  Service<AppDatabase> xdd;
  InputField *nameField;
  InputField *urlField;
  AppTurbobox *appField;

public:
  CreateQuickLinkCommand(AppWindow *app);

  void onActionActivated(std::shared_ptr<IAction> action) override;
  void onAttach() override;
  QIcon icon() override;
  QString name() override;
};
