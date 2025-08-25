#include "services/extension-boilerplate-generator/extension-boilerplate-generator.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/form-field.hpp"
#include "ui/form/form.hpp"
#include "ui/form/text-area.hpp"
#include "ui/views/form-view.hpp"
#include "utils/utils.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include <filesystem>
#include <qjsonvalue.h>
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
    auto isMinStrLen = [](int min) {
      return [min](const QJsonValue &value) {
        return value.toString().size() < min ? QString("Min. %1 chars").arg(min) : "";
      };
    };
    auto isValidDir = [](const QJsonValue &value) {
      namespace fs = std::filesystem;
      std::error_code ec;
      fs::path path = expandPath(value.toString().toStdString());

      if (!fs::is_directory(path, ec)) { return "Must exist"; }

      return "";
    };

    form()->clearFields();
    auto authorField = form()->addField("Author", m_username);

    authorField->setInfo(
        "For now, you can set any username. The username will be used in the command deeplink, and "
        "may carry more significance in the future.");

    authorField->setValidator(isMinStrLen(3));

    form()->addSeparator();

    auto title = form()->addField("Extension Title", m_title);
    auto description = form()->addField("Description", m_description);
    auto location = form()->addField("Location", m_location);

    title->setValidator(isMinStrLen(3));
    description->setValidator(isMinStrLen(16));
    location->setValidator(isValidDir);

    for (const auto &cmd : m_commands) {
      form()->addSeparator();
      auto title = form()->addField("Command Title", cmd.m_title);
      auto subtitle = form()->addField("Subtitle", cmd.m_subtitle);
      auto description = form()->addField("Description", cmd.m_description);
      auto tmpl = form()->addField("Template", cmd.m_template);

      title->setValidator(isMinStrLen(3));
      subtitle->setValidator(isMinStrLen(3));
      description->setValidator(isMinStrLen(3));
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
    auto toast = context()->services->toastService();

    if (!form()->validate()) {
      toast->failure("Form has errors");
      return;
    }

    auto cfg = getConfig();
    ExtensionBoilerplateGenerator gen;
    std::filesystem::path targetDir = expandPath(m_location->text().toStdString());

    if (auto v = gen.generate(targetDir, cfg); !v) {
      toast->failure("Failed to create extension");
      qCritical() << "Failed to create extension with error" << v.error();
      return;
    }

    toast->success("Created extension");
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
