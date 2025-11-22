#include "extend/grid-model.hpp"
#include "extend/action-model.hpp"
#include "extend/color-model.hpp"
#include "extend/empty-view-model.hpp"
#include "extend/image-model.hpp"
#include "extend/pagination-model.hpp"
#include "utils.hpp"
#include <qjsonvalue.h>
#include <ranges>
#include <qjsonarray.h>
#include <qjsonobject.h>

GridItemViewModel GridModelParser::parseListItem(const QJsonObject &instance, size_t index) {
  GridItemViewModel model;
  auto props = instance.value("props").toObject();
  auto children = instance.value("children").toArray();

  model.id = props["id"].toString(QString::number(index));
  model.title = props["title"].toString();
  model.subtitle = props["subtitle"].toString();

  auto content = props.value("content");

  if (content.isObject()) {
    auto obj = content.toObject();

    if (obj.contains("tooltip")) { model.tooltip = obj.value("tooltip").toString(); }

    if (obj.contains("color")) {
      model.content = ColorLikeModelParser().parse(obj.value("color"));
    } else if (obj.contains("value")) {
      model.content = ImageModelParser().parse(obj.value("value"));
    } else {
      model.content = ImageModelParser().parse(content);
    }
  } else {
    model.content = ImageModelParser().parse(content);
  }

  if (props.contains("keywords")) {
    model.keywords =
        ranges_to<std::vector>(props.value("keywords").toArray() |
                               std::views::transform([](const QJsonValue &&a) { return a.toString(); }));
  }

  for (const auto &child : children) {
    auto obj = child.toObject();
    auto type = obj["type"].toString();

    if (type == "action-panel") { model.actionPannel = ActionPannelParser().parse(obj); }
  }

  return model;
}

GridSectionModel GridModelParser::parseSection(const QJsonObject &instance) {
  GridSectionModel model;
  size_t index = 0;
  auto props = instance.value("props").toObject();
  auto arr = instance.value("children").toArray();

  model.title = props.value("title").toString();
  model.subtitle = props.value("subtitle").toString();

  if (props.contains("fit")) { model.fit = parseFit(props.value("fit").toString()); }
  if (props.contains("aspectRatio")) { model.aspectRatio = props.value("aspectRatio").toDouble(1); }
  if (props.contains("columns")) { model.columns = props.value("columns").toInt(); }
  if (auto inset = props.value("inset"); inset.isString()) { model.inset = parseInset(inset.toString()); }

  model.children.reserve(arr.size());

  for (const auto &child : arr) {
    auto obj = child.toObject();
    auto type = obj.value("type").toString();

    if (type == "grid-item") {
      auto item = parseListItem(obj, index);

      model.children.push_back(item);
    }

    ++index;
  }

  return model;
}

GridItemContentWidget::Inset GridModelParser::parseInset(const QString &s) {
  using Inset = GridItemContentWidget::Inset;

  if (s == "zero") return Inset::None;
  if (s == "small") return Inset::Small;
  if (s == "medium") return Inset::Medium;
  if (s == "large") return Inset::Large;

  return Inset::Small;
}

ObjectFit GridModelParser::parseFit(const QString &fit) {
  if (fit == "fill") return ObjectFit::Fill;
  return ObjectFit::Contain;
}

GridModel GridModelParser::parse(const QJsonObject &instance) {
  GridModel model;
  auto props = instance.value("props").toObject();
  // no builtin filtering by default if onSearchTextChange handler is specified
  bool defaultFiltering = !props.contains("onSearchTextChange");

  model.dirty = instance.value("dirty").toBool(true);
  model.isLoading = props["isLoading"].toBool(false);
  model.throttle = props["throttle"].toBool(false);
  model.fit = parseFit(props.value("fit").toString());
  model.aspectRatio = props.value("aspectRatio").toDouble(1);
  model.searchPlaceholderText = props["searchBarPlaceholder"].toString();
  model.filtering = props["filtering"].toBool(defaultFiltering);

  if (auto inset = props.value("inset"); inset.isString()) { model.inset = parseInset(inset.toString()); }
  if (auto cols = props.value("columns"); cols.isDouble()) { model.columns = cols.toInt(); }

  if (props.contains("navigationTitle")) {
    model.navigationTitle = props.value("navigationTitle").toString();
  }

  if (props.contains("onSearchTextChange")) {
    model.onSearchTextChange = props.value("onSearchTextChange").toString();
  }

  if (props.contains("onSelectionChange")) {
    model.onSelectionChanged = props.value("onSelectionChange").toString();
  }

  if (props.contains("selectedItemId")) { model.selectedItemId = props.value("selectedItemId").toString(); }
  if (props.contains("searchText")) { model.searchText = props.value("searchText").toString(); }
  if (props.contains("pagination")) {
    model.pagination = PaginationModel::fromJson(props.value("pagination").toObject());
  }

  size_t index = 0;

  for (const auto &child : instance.value("children").toArray()) {
    auto childObj = child.toObject();
    auto type = childObj.value("type").toString();

    if (type == "action-panel") { model.actions = ActionPannelParser().parse(childObj); }

    if (type == "grid-item") {
      auto item = parseListItem(childObj, index);

      model.items.push_back(item);
    }

    if (type == "grid-section") {
      auto section = parseSection(childObj);

      model.items.push_back(section);
    }

    if (type == "dropdown") { model.searchBarAccessory = DropdownModel::fromJson(childObj); }

    if (type == "empty-view") { model.emptyView = EmptyViewModelParser().parse(childObj); }

    ++index;
  }

  return model;
}

GridModelParser::GridModelParser() {}
