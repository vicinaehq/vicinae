#include "ui/form/base-input.hpp"
#include "ui/form/form-field.hpp"
#include "ui/form/form.hpp"
#include "ui/form/text-area.hpp"
#include "ui/views/form-view.hpp"

struct CreateExtensionCommandFrame {
  BaseInput *m_title = new BaseInput;
  BaseInput *m_subtitle = new BaseInput;
  TextArea *m_description = new TextArea;
};

class CreateExtensionView : public ManagedFormView {
  BaseInput *m_username = new BaseInput;
  BaseInput *m_title = new BaseInput;
  TextArea *m_description = new TextArea;
  BaseInput *m_location = new BaseInput;

  std::vector<CreateExtensionCommandFrame> m_commands;

  void generateForm() {
    form()->clearFields();
    form()
        ->addField("Author", m_username)
        ->setInfo("For now, you can set any username. The username will be used in the command deeplink, and "
                  "may carry more significance in the future.");

    form()->addSeparator();
    form()->addField("Extension Title", m_title);
    form()->addField("Description", m_description);
    form()->addField("Location", m_location);

    for (const auto &cmd : m_commands) {
      form()->addSeparator();
      form()->addField("Command Title", cmd.m_title);
      form()->addField("Subtitle", cmd.m_subtitle);
      form()->addField("Description", cmd.m_description);
    }
  }

public:
  QString submitTitle() const override { return "Create extension"; }

  void onSubmit() override { qDebug() << "submit"; }

  CreateExtensionView() {
    m_username->setPlaceholderText("Username");
    m_title->setPlaceholderText("My Extension");
    m_description->setPlaceholderText("An extension that does super cool things");

    // at least one command
    m_commands.push_back({});

    generateForm();
  }
};
