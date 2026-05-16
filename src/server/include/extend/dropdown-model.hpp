#pragma once
#include "extend/image-model.hpp"
#include "extend/node-props.hpp"
#include "extend/node-tree.hpp"
#include <QDebug>
#include <optional>
#include <qstring.h>
#include <variant>

struct DropdownModel {
  struct Item {
    QString title;
    QString value;
    std::optional<ImageLikeModel> icon;
    std::vector<QString> keywords;

    static Item fromNode(const Node &node) {
      Item model;
      const auto &props = node.props;

      model.title = QString::fromStdString(node_props::getStringOr(props, "title"));
      model.value = QString::fromStdString(node_props::getStringOr(props, "value"));

      if (auto *v = node_props::get(props, "icon")) { model.icon = ImageModelParser().parse(*v); }

      if (auto *arr = node_props::getArray(props, "keywords")) {
        model.keywords.reserve(arr->size());
        for (const auto &keyword : *arr) {
          if (keyword.is_string()) { model.keywords.push_back(QString::fromStdString(keyword.get_string())); }
        }
      }

      return model;
    }
  };

  struct Section {
    QString title;
    std::vector<Item> items;

    static Section fromNode(const Node &node, const NodeTree &tree) {
      Section section;
      const auto &props = node.props;

      section.title = QString::fromStdString(node_props::getStringOr(props, "title"));
      section.items.reserve(node.childIds.size());

      forEachChild(node, tree, [&](const Node &child) {
        if (child.type == "dropdown-item") { section.items.push_back(Item::fromNode(child)); }
      });

      return section;
    }
  };

  using Child = std::variant<Item, Section>;

  struct Filtering {
    bool keepSectionOrder;
    bool enabled;
  };

  bool dirty = true;
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

  static DropdownModel fromNode(const Node &node, const NodeTree &tree) {
    DropdownModel model;
    const auto &props = node.props;

    if (auto sv = node_props::getString(props, "tooltip"))
      model.tooltip = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
    if (auto sv = node_props::getString(props, "defaultValue"))
      model.defaultValue = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
    if (auto sv = node_props::getString(props, "id"))
      model.id = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
    if (auto sv = node_props::getString(props, "onChange"))
      model.onChange = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
    if (auto sv = node_props::getString(props, "onSearchTextChange"))
      model.onSearchTextChange = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
    if (auto sv = node_props::getString(props, "placeholder"))
      model.placeholder = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
    if (auto sv = node_props::getString(props, "value"))
      model.value = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));

    model.storeValue = node_props::getBool(props, "storeValue", true);
    model.throttle = node_props::getBool(props, "throttle");
    model.isLoading = node_props::getBool(props, "isLoading");
    model.filtering = {
        .keepSectionOrder = true,
        .enabled = node_props::getBool(props, "filtering", true),
    };

    model.children.reserve(node.childIds.size());

    forEachChild(node, tree, [&](const Node &child) {
      if (child.type == "dropdown-item") {
        model.children.push_back(Item::fromNode(child));
      } else if (child.type == "dropdown-section") {
        model.children.push_back(Section::fromNode(child, tree));
      } else {
        qWarning() << "DropdownModel: unhandled child type" << QString::fromStdString(child.type);
      }
    });

    return model;
  }
};
