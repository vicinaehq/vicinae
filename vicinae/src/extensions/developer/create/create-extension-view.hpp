#include "services/extension-boilerplate-generator/extension-boilerplate-generator.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/form-field.hpp"
#include "ui/form/form.hpp"
#include "ui/form/text-area.hpp"
#include "ui/views/form-view.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include <qlogging.h>
#include <ranges>

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

  ExtensionBoilerplateConfig::CommandConfig mapCommandToConfig(const CreateExtensionCommandFrame &frame) {
    ExtensionBoilerplateConfig::CommandConfig cfg;

    cfg.title = frame.m_title->text();
    cfg.subtitle = frame.m_subtitle->text();
    cfg.description = frame.m_description->text();

    return cfg;
  }

  ExtensionBoilerplateConfig getConfig() {
    ExtensionBoilerplateConfig cfg;

    cfg.author = m_username->text();
    cfg.title = m_title->text();
    cfg.description = m_description->text();
    cfg.commands = m_commands |
                   std::views::transform([this](auto &&cfg) { return mapCommandToConfig(cfg); }) |
                   std::ranges::to<std::vector>();

    return cfg;
  }

  void onSubmit() override {
    auto cfg = getConfig();
    ExtensionBoilerplateGenerator gen;
    std::filesystem::path targetDir = m_location->text().toStdString();

    if (auto v = gen.generate(targetDir, cfg); !v) {
      context()->services->toastService()->failure("Failed to create extension");
      qCritical() << "Failed to create extension with error" << v.error();
      return;
    }
  }

public:
  QString submitTitle() const override { return "Create extension"; }

  CreateExtensionView() {
    m_username->setPlaceholderText("Username");
    m_title->setPlaceholderText("My Extension");
    m_description->setPlaceholderText("An extension that does super cool things");

    // at least one command
    m_commands.push_back({});

    generateForm();
  }
};
