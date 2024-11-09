#include "render.hpp"
#include "extension_manager.hpp"
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <jsoncpp/json/value.h>
#include <qabstractspinbox.h>
#include <qboxlayout.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistwidget.h>
#include <qobjectdefs.h>
#include <qwidget.h>
#include <stdexcept>

Component *createComponent(std::string_view type, Json::Value props,
                           Json::Value children) {
  std::cout << "[+Component] " << type << std::endl;
  if (type == "container")
    return new ContainerComponent(props, children);
  if (type == "SearchInput")
    return new SearchInputComponent(props, children);
  if (type == "List")
    return new ListComponent(props, children);
  if (type == "ListItem")
    return new ListItemComponent(props, children);
  if (type == "Image")
    return new ImageComponent(props, children);

  throw std::runtime_error("Tried to create unknown component of type " +
                           std::string(type));
}

Component *renderComponentTree(Component *root, Json::Value newNode) {
  auto type = newNode["type"].asString();
  auto props = newNode["props"];
  auto children = newNode["children"];

  if (!root || type != root->type) {
    root = createComponent(type, props, children);
  }

  root->updateProps(props);

  auto minSize = std::min((size_t)children.size(), root->children.size());

  // update existing children
  for (int i = 0; i != minSize; ++i) {
    auto from = root->children.at(i);
    auto to = renderComponentTree(from, children[i]);

    if (from != to) {
      root->replaceChild(from, to);
      delete from;
    }
  }

  for (int i = minSize; i < children.size(); ++i) {
    auto child = children[i];
    auto component = createComponent(child["type"].asString(), child["props"],
                                     child["children"]);

    root->appendChild(component);
  }

  while (root->children.size() > children.size()) {
    if (Component *pop = root->popChild())
      delete pop;
  }

  return root;
}
