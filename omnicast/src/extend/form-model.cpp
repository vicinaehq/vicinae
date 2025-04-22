#include "extend/form-model.hpp"
#include <qjsonobject.h>

const static std::vector<QString> fieldTypes = {"dropdown-field", "password-field",    "text-field",
                                                "checkbox-field", "date-picker-field", "text-area-field"};

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

    if (type == "action-panel") {
      model.actions = ActionPannelParser().parse(obj);
    } else if (type == "form-separator") {
      model.items.push_back(Separator{});
    } else if (type == "form-description") {
      Description desc;

      desc.text = props.value("text").toString();
      if (props.contains("title")) desc.title = props.value("title").toString();

      model.items.push_back(desc);

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
        model.items.emplace_back(TextFieldModel(base));
      } else if (*it == "password-field") {
        model.items.emplace_back(PasswordFieldModel(base));
      } else if (*it == "checkbox-field") {
        model.items.emplace_back(CheckboxFieldModel(base));
      } else if (*it == "date-picker-field") {
        model.items.emplace_back(DatePickerFieldModel(base));
      } else if (*it == "text-area-field") {
        model.items.emplace_back(TextAreaFieldModel(base));
      }
    } else {
      qWarning() << "Unknown form children of type" << type;
    }
  }

  return model;
}
