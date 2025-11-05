#include "extend/form-model.hpp"
#include <qjsonobject.h>

const static std::vector<QString> fieldTypes = {"dropdown-field",   "password-field",    "text-field",
                                                "checkbox-field",   "date-picker-field", "text-area-field",
                                                "file-picker-field"};

FormModel FormModel::fromJson(const QJsonObject &json) {
  FormModel model;
  auto props = json.value("props").toObject();
  auto children = json.value("children").toArray();

  model.isLoading = props.value("isLoading").toBool(false);
  model.enableDrafts = props.value("enableDrafts").toBool(false);

  if (props.contains("navigationTitle")) {
    model.navigationTitle = props.value("navigationTtile").toString();
  }

  model.items.reserve(children.size());

  for (const auto &child : children) {
    auto obj = child.toObject();
    auto type = obj.value("type").toString();
    auto props = obj.value("props").toObject();
    auto children = obj.value("children").toArray();

    if (type == "action-panel") {
      model.actions = ActionPannelParser().parse(obj);
    } else if (type == "separator") {
      model.items.push_back(Separator{});
    } else if (type == "form-description") {
      Description desc;

      desc.text = props.value("text").toString();
      if (props.contains("title")) desc.title = props.value("title").toString();

      model.items.push_back(desc);

    } else if (type == "link-accessory") {
      FormModel::LinkAccessoryModel link;
      link.text = props.value("text").toString();
      link.target = props.value("target").toString();
      model.searchBarAccessory = link;

    } else if (auto it = std::find(fieldTypes.begin(), fieldTypes.end(), type); it != fieldTypes.end()) {
      FieldBase base;

      if (!props.contains("id")) {
        qWarning() << "Found form field" << *it << "without ID field: skipping";
        continue;
      }

      base.id = props.value("id").toString();
      base.storeValue = props.value("storeValue").toBool();
      base.autoFocus = props.value("autoFocus").toBool();

      if (props.contains("title")) base.title = props.value("title").toString();
      if (props.contains("value")) base.value = props.value("value");
      if (props.contains("error")) base.error = props.value("error").toString();
      if (props.contains("info")) base.info = props.value("info").toString();
      if (props.contains("onBlur")) base.onBlur = props.value("onBlur").toString();
      if (props.contains("onFocus")) base.onFocus = props.value("onFocus").toString();
      if (props.contains("onChange")) base.onChange = props.value("onChange").toString();
      if (props.contains("defaultValue")) base.defaultValue = props.value("defaultValue").toString();

      qDebug() << "registered" << base.id << base.onChange;

      if (*it == "text-field") {
        model.items.emplace_back(std::make_shared<TextField>(base));
      } else if (*it == "password-field") {
        model.items.emplace_back(std::make_shared<PasswordField>(base));
      } else if (*it == "checkbox-field") {
        auto checkbox = std::make_shared<CheckboxField>(base);
        if (props.contains("label")) checkbox->m_label = props.value("label").toString();
        model.items.emplace_back(checkbox);
      } else if (*it == "date-picker-field") {
        auto dp = std::make_shared<DatePickerField>(base);
        if (props.contains("min")) dp->min = props.value("min").toString();
        if (props.contains("max")) dp->max = props.value("max").toString();
        if (props.contains("type")) dp->type = props.value("type").toString();
        model.items.emplace_back(dp);
      } else if (*it == "text-area-field") {
        auto ta = std::make_shared<TextAreaField>(base);
        if (props.contains("placeholder")) ta->placeholder = props.value("placeholder").toString();
        model.items.emplace_back(ta);
      } else if (*it == "dropdown-field") {
        auto dropdown = std::make_shared<DropdownField>(base);

        dropdown->m_items.reserve(children.size());
        dropdown->throttle = props.value("throttle").toBool(false);
        dropdown->isLoading = props.value("isLoading").toBool(false);

        if (props.contains("tooltip")) { dropdown->tooltip = props.value("tooltip").toString(); }

        if (props.contains("onSearchTextChange"))
          dropdown->onSearchTextChange = props.value("onSearchTextChange").toString();

        dropdown->filtering = props.value("filtering").toBool(!dropdown->onSearchTextChange.has_value());

        for (const auto &child : children) {
          dropdown->m_items.emplace_back(DropdownModel::childFromJson(child.toObject()));
        }

        model.items.emplace_back(dropdown);
      } else if (*it == "file-picker-field") {
        auto filePicker = std::make_shared<FilePickerField>(base);

        if (props.contains("allowMultipleSelection"))
          filePicker->allowMultipleSelection = props.value("allowMultipleSelection").toBool();
        if (props.contains("canChooseDirectories"))
          filePicker->canChooseDirectories = props.value("canChooseDirectories").toBool();
        if (props.contains("canChooseFiles"))
          filePicker->canChooseFiles = props.value("canChooseFiles").toBool();
        if (props.contains("showHiddenFiles"))
          filePicker->showHiddenFiles = props.value("showHiddenFiles").toBool();

        model.items.emplace_back(filePicker);
      }
    } else {
      qWarning() << "Unknown form children of type" << type;
    }
  }

  return model;
}
