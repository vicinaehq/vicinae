#include "extend/form-model.hpp"
#include "extend/node-props.hpp"
#include "lib/glaze-qt.hpp"
#include <algorithm>

static const std::vector<std::string> FIELD_TYPES = {
    "dropdown-field",    "password-field",  "text-field",       "checkbox-field",
    "date-picker-field", "text-area-field", "file-picker-field"};

FormModel FormModel::fromNode(const Node &node, const NodeTree &tree) {
  FormModel model;
  const auto &props = node.props;

  model.isLoading = node_props::getBool(props, "isLoading");
  model.enableDrafts = node_props::getBool(props, "enableDrafts");

  if (auto sv = node_props::getString(props, "navigationTitle")) {
    model.navigationTitle = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
  }

  model.items.reserve(node.childIds.size());

  forEachChild(node, tree, [&](const Node &child) {
    const auto &childProps = child.props;

    if (child.type == "action-panel") {
      model.actions = ActionPannelParser().parse(child, tree);

    } else if (child.type == "separator") {
      model.items.emplace_back(Separator{});

    } else if (child.type == "form-description") {
      Description desc;
      desc.text = QString::fromStdString(node_props::getStringOr(childProps, "text"));
      if (auto sv = node_props::getString(childProps, "title")) {
        desc.title = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
      }
      model.items.emplace_back(desc);

    } else if (child.type == "link-accessory") {
      FormModel::LinkAccessoryModel link;
      link.text = QString::fromStdString(node_props::getStringOr(childProps, "text"));
      link.target = QString::fromStdString(node_props::getStringOr(childProps, "target"));
      model.searchBarAccessory = link;

    } else if (auto it = std::ranges::find(FIELD_TYPES, child.type); it != FIELD_TYPES.end()) {
      FieldBase base;

      if (!node_props::has(childProps, "id")) {
        qWarning() << "Found form field" << QString::fromStdString(*it) << "without ID field: skipping";
        return;
      }

      base.id = QString::fromStdString(node_props::getStringOr(childProps, "id"));
      base.storeValue = node_props::getBool(childProps, "storeValue", true);
      base.autoFocus = node_props::getBool(childProps, "autoFocus");

      if (auto sv = node_props::getString(childProps, "title")) {
        base.title = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
      }

      if (auto *v = node_props::get(childProps, "value")) {
        if (v->is_object()) {
          const auto &valObj = v->get_object();
          if (valObj.contains("eventCount")) {
            auto qval = glazeToQJsonValue(*v);
            auto innerVal = qval.toObject().value("value");
            base.value = parseEventCounted(valObj, std::move(innerVal));
          } else {
            base.value = EventCounted<QJsonValue>{glazeToQJsonValue(*v), EVENT_COUNT_UNTRACKED};
          }
        } else {
          base.value = EventCounted<QJsonValue>{glazeToQJsonValue(*v), EVENT_COUNT_UNTRACKED};
        }
      }

      if (auto sv = node_props::getString(childProps, "error")) {
        base.error = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
      }
      if (auto sv = node_props::getString(childProps, "info")) {
        base.info = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
      }
      if (auto sv = node_props::getString(childProps, "onBlur")) {
        base.onBlur = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
      }
      if (auto sv = node_props::getString(childProps, "onFocus")) {
        base.onFocus = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
      }
      if (auto sv = node_props::getString(childProps, "onChange")) {
        base.onChange = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
      }
      if (auto *v = node_props::get(childProps, "defaultValue")) {
        base.defaultValue = glazeToQJsonValue(*v);
      }

      if (*it == "text-field") {
        auto tf = std::make_shared<TextField>(base);
        if (auto sv = node_props::getString(childProps, "placeholder")) {
          tf->m_placeholder = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
        }
        model.items.emplace_back(tf);

      } else if (*it == "password-field") {
        auto pf = std::make_shared<PasswordField>(base);
        if (auto sv = node_props::getString(childProps, "placeholder")) {
          pf->m_placeholder = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
        }
        model.items.emplace_back(pf);

      } else if (*it == "checkbox-field") {
        auto checkbox = std::make_shared<CheckboxField>(base);
        if (auto sv = node_props::getString(childProps, "label")) {
          checkbox->m_label = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
        }
        model.items.emplace_back(checkbox);

      } else if (*it == "date-picker-field") {
        auto dp = std::make_shared<DatePickerField>(base);
        if (auto sv = node_props::getString(childProps, "min")) {
          dp->min = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
        }
        if (auto sv = node_props::getString(childProps, "max")) {
          dp->max = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
        }
        if (auto sv = node_props::getString(childProps, "type")) {
          dp->type = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
        }
        model.items.emplace_back(dp);

      } else if (*it == "text-area-field") {
        auto ta = std::make_shared<TextAreaField>(base);
        if (auto sv = node_props::getString(childProps, "placeholder")) {
          ta->m_placeholder = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
        }
        model.items.emplace_back(ta);

      } else if (*it == "dropdown-field") {
        auto dropdown = std::make_shared<DropdownField>(base);

        dropdown->m_items.reserve(child.childIds.size());
        dropdown->throttle = node_props::getBool(childProps, "throttle");
        dropdown->isLoading = node_props::getBool(childProps, "isLoading");

        if (auto sv = node_props::getString(childProps, "placeholder")) {
          dropdown->m_placeholder = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
        }
        if (auto sv = node_props::getString(childProps, "tooltip")) {
          dropdown->tooltip = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
        }
        if (auto sv = node_props::getString(childProps, "onSearchTextChange")) {
          dropdown->onSearchTextChange = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
        }

        dropdown->filtering =
            node_props::getBool(childProps, "filtering", !dropdown->onSearchTextChange.has_value());

        forEachChild(child, tree, [&](const Node &dropdownChild) {
          if (dropdownChild.type == "dropdown-item") {
            dropdown->m_items.emplace_back(DropdownModel::Item::fromNode(dropdownChild));
          } else if (dropdownChild.type == "dropdown-section") {
            dropdown->m_items.emplace_back(DropdownModel::Section::fromNode(dropdownChild, tree));
          }
        });

        model.items.emplace_back(dropdown);

      } else if (*it == "file-picker-field") {
        auto filePicker = std::make_shared<FilePickerField>(base);

        filePicker->allowMultipleSelection = node_props::getBool(childProps, "allowMultipleSelection");
        filePicker->canChooseDirectories = node_props::getBool(childProps, "canChooseDirectories");
        filePicker->canChooseFiles = node_props::getBool(childProps, "canChooseFiles", true);
        filePicker->showHiddenFiles = node_props::getBool(childProps, "showHiddenFiles");

        model.items.emplace_back(filePicker);
      }
    } else {
      qWarning() << "Unknown form children of type" << QString::fromStdString(child.type);
    }
  });

  return model;
}
