#pragma once
#include "extend/image-model.hpp"
#include <optional>
#include <qjsonobject.h>
#include <qlogging.h>
#include <qobject.h>
#include <qstring.h>
#include <qjsonarray.h>

struct DropdownModel {
  struct Item {
    QString title;
    QString value;
    std::optional<ImageLikeModel> icon;
    std::vector<QString> keywords;

    static Item fromJson(const QJsonObject &json) {
      Item model;
      auto props = json.value("props").toObject();

      model.title = props.value("title").toString();
      model.value = props.value("value").toString();

      if (props.contains("icon")) { model.icon = ImageModelParser().parse(props.value("icon")); }

      auto keywords = props.value("keywords").toArray();

      model.keywords.reserve(keywords.size());

      for (const auto &keyword : keywords) {
        model.keywords.push_back(keyword.toString());
      }

      return model;
    }
  };

  struct Section {
    QString title;
    std::vector<Item> items;

    static Section fromJson(const QJsonObject &json) {
      Section section;
      auto props = json.value("props").toObject();
      auto children = json.value("children").toArray();

      section.title = props.value("title").toString();
      section.items.reserve(children.size());

      for (const auto &child : children) {
        auto obj = child.toObject();
        auto type = obj.value("type").toString();

        if (type == "dropdown-item") { section.items.push_back(Item::fromJson(obj)); }
      }

      return section;
    }
  };

  using Child = std::variant<Item, Section>;

  struct Filtering {
    bool keepSectionOrder;
    bool enabled;
  };

  bool dirty;
  std::optional<QString> tooltip;
  std::optional<QString> defaultValue;
  std::optional<QString> id;
  std::optional<QString> onChange;
  std::optional<QString> onSearchTextChange;
  std::optional<QString> placeholder;
  std::optional<QString> value;
  std::vector<Child> children;
  bool storeValue = true;
  bool throttle = false;
  Filtering filtering;
  bool isLoading;

  static Child childFromJson(const QJsonObject &json) {
    auto type = json.value("type").toString();

    if (type == "dropdown-item") {
      return Item::fromJson(json);
    } else if (type == "dropdown-section") {
      return Section::fromJson(json);
    } else {
      qWarning() << "DropdownModel: unhandled child type" << type;
    }

    return {};
  }

  static DropdownModel fromJson(const QJsonObject &json) {
    DropdownModel model;
    auto props = json.value("props").toObject();
    auto children = json.value("children").toArray();

    model.dirty = json.value("dirty").toBool(false);

    if (props.contains("tooltip")) model.tooltip = props.value("tooltip").toString();
    if (props.contains("defaultValue")) model.defaultValue = props.value("defaultValue").toString();
    if (props.contains("id")) model.id = props.value("id").toString();
    if (props.contains("onChange")) model.onChange = props.value("onChange").toString();
    if (props.contains("onSearchTextChange"))
      model.onSearchTextChange = props.value("onSearchTextChange").toString();
    if (props.contains("placeholder")) model.placeholder = props.value("placeholder").toString();
    if (props.contains("value")) model.value = props.value("value").toString();

    model.storeValue = props.value("storeValue").toBool(true);
    model.throttle = props.value("throttle").toBool(false);
    model.isLoading = props.value("isLoading").toBool(false);
    model.filtering = {
        .keepSectionOrder = true,
        .enabled = props.value("filtering").toBool(true),
    };

    model.children.reserve(children.size());

    for (const auto &child : children) {
      auto obj = child.toObject();
      auto type = obj.value("type").toString();
      auto props = obj.value("props").toObject();

      if (type == "dropdown-item") {
        model.children.push_back(Item::fromJson(obj));
      } else if (type == "dropdown-section") {
        model.children.push_back(Section::fromJson(obj));
      } else {
        qWarning() << "DropdownModel: unhandled child type" << type;
      }
    }

    return model;
  }
};
