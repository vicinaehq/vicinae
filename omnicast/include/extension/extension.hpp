#pragma once
#include "command-database.hpp"
#include "omni-icon.hpp"
#include "preference.hpp"
#include <filesystem>
#include <qjsonobject.h>
#include <qstring.h>
#include <filesystem>
#include <qstringview.h>
#include <qjsonarray.h>

class ExtensionAction : public AbstractAction {
  ActionModel _model;

public:
  void execute(AppWindow &app) override {}

  const ActionModel &model() const { return _model; }

  ExtensionAction(const ActionModel &model)
      : AbstractAction(model.title, model.icon ? OmniIconUrl(*model.icon) : BuiltinOmniIconUrl("pen")),
        _model(model) {
    shortcut = _model.shortcut;
  }
};

class Extension : public AbstractCommandRepository {
public:
  struct CommandBase : public BuiltinCommand {
    QString _name;
    QString title;
    QString subtitle;
    QString description;
    QString mode;
  };

private:
  QString _id;
  QString _name;
  QString _title;
  QString _icon;
  std::filesystem::path _path;
  PreferenceList _preferences;
  std::vector<std::shared_ptr<AbstractCmd>> _commands;

  explicit Extension(const QJsonObject &object);

public:
  Extension() {}

  static Extension fromObject(const QJsonObject &object);

  static CommandArgument parseArgumentFromObject(const QJsonObject &obj) {
    CommandArgument arg;
    QString type = obj.value("type").toString();

    if (type == "text") arg.type = CommandArgument::Text;
    if (type == "password") arg.type = CommandArgument::Password;
    if (type == "dropdown") arg.type = CommandArgument::Dropdown;

    arg.name = obj.value("name").toString();
    arg.placeholder = obj.value("placeholder").toString();
    arg.required = obj.value("required").toBool(false);

    if (type == "dropdown") {
      auto data = obj.value("data").toArray();
      std::vector<CommandArgument::DropdownData> options;

      options.reserve(data.size());

      for (const auto &child : data) {
        auto obj = child.toObject();

        options.push_back({.title = obj["title"].toString(), .value = obj["value"].toString()});
      }

      arg.data = options;
    }

    return arg;
  }

  static Preference parsePreferenceFromObject(const QJsonObject &obj) {
    auto type = obj["type"].toString();
    Preference base;

    base.setTitle(obj["title"].toString());
    base.setDescription(obj["description"].toString());
    base.setName(obj["name"].toString());
    base.setPlaceholder(obj["placeholder"].toString());
    base.setRequired(obj["required"].toBool());
    base.setDefaultValue(obj.value("default").toString());

    if (type == "textfield") { base.setData(Preference::TextData()); }
    if (type == "password") { base.setData(Preference::PasswordData()); }

    if (type == "checkbox") {
      auto checkbox = Preference::CheckboxData(obj["label"].toString());

      base.setData(checkbox);
    }

    if (type == "appPicker") {
      // XXX: implement later - if we really need it
    }

    if (type == "dropdown") {
      auto data = obj["data"].toArray();
      std::vector<Preference::DropdownData::Option> options;

      options.reserve(data.size());

      for (const auto &child : data) {
        auto obj = child.toObject();

        options.push_back({.title = obj["title"].toString(), .value = obj["value"].toString()});
      }

      base.setData(Preference::DropdownData{options});
    }

    return base;
  }

  QString id() const override;
  QString name() const override;
  OmniIconUrl iconUrl() const override;
  std::filesystem::path assetDirectory() const;
  std::filesystem::path installedPath() const;
  PreferenceList preferences() const override;
  std::vector<std::shared_ptr<AbstractCmd>> commands() const override;
};
