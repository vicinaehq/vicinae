#include "services/extension-boilerplate-generator/extension-boilerplate-generator.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/form-field.hpp"
#include "ui/form/form.hpp"
#include "ui/form/text-area.hpp"
#include "ui/views/form-view.hpp"
#include "utils/utils.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include <qlogging.h>
#include <ranges>
#include "ui/preference-dropdown/preference-dropdown.hpp"

class CommandTemplateDropdown : public PreferenceDropdown {
public:
  CommandTemplateDropdown() {
    ExtensionBoilerplateGenerator gen;
    auto tr = [](auto &&tmpl) {
      return Preference::DropdownData::Option({.title = tmpl.name, .value = tmpl.resource});
    };
    auto tmpls = gen.commandBoilerplates();

    setOptions(tmpls | std::views::transform(tr) | std::ranges::to<std::vector>());

    if (!tmpls.empty()) { setValue(tmpls.at(0).resource); }
  }
};

struct CreateExtensionCommandFrame {
  BaseInput *m_title = new BaseInput;
  BaseInput *m_subtitle = new BaseInput;
  TextArea *m_description = new TextArea;
  CommandTemplateDropdown *m_template = new CommandTemplateDropdown;

  CreateExtensionCommandFrame() {
    m_title->setPlaceholderText("My Wonderful Command");
    m_subtitle->setPlaceholderText("An helpful subtitle");
    m_description->setPlaceholderText("My command does this, and that...");
  }
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
      form()->addField("Template", cmd.m_template);
    }
  }

  ExtensionBoilerplateConfig::CommandConfig mapCommandToConfig(const CreateExtensionCommandFrame &frame) {
    ExtensionBoilerplateConfig::CommandConfig cfg;

    cfg.title = frame.m_title->text();
    cfg.subtitle = frame.m_subtitle->text();
    cfg.description = frame.m_description->text();

    if (auto tmpl = frame.m_template->value()) { cfg.templateId = tmpl->id(); }

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
    std::filesystem::path targetDir = expandPath(m_location->text().toStdString());

    if (auto v = gen.generate(targetDir, cfg); !v) {
      context()->services->toastService()->failure("Failed to create extension");
      qCritical() << "Failed to create extension with error" << v.error();
      return;
    }

    context()->services->toastService()->success("Created extension");
  }

public:
  QString submitTitle() const override { return "Create extension"; }

  CreateExtensionView() {
    m_username->setPlaceholderText("Username");
    m_title->setPlaceholderText("My Extension");
    m_description->setPlaceholderText("An extension that does super cool things");
    m_location->setPlaceholderText("~/code/vicinae-extensions");

    // at least one command
    m_commands.push_back({});

    generateForm();
  }
};
