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

Component *createComponent(ExtensionManager *manager, std::string_view type,
                           Json::Value props, Json::Value children,
                           QWidget *parent) {
  std::cout << "[+Component] " << type << std::endl;
  if (type == "container")
    return new ContainerComponent(manager, props, children, parent);
  if (type == "SearchInput")
    return new SearchInputComponent(manager, props, children);
  if (type == "List")
    return new ListComponent(manager, props, children, parent);
  if (type == "ListItem")
    return new ListItemComponent(manager, props, children);
  if (type == "Label")
    return new LabelComponent(manager, props, children, parent);
  if (type == "Image")
    return new ImageComponent(props, children, parent);

  throw std::runtime_error("Tried to create unknown component of type " +
                           std::string(type));
}

void deleteComponent(Component *component) {
  std::cout << "[-Component] " << component->type << std::endl;
  delete component;
}

Component *renderComponentTree(ExtensionManager *manager, Component *root,
                               Json::Value newNode) {
  auto type = newNode["type"].asString();
  auto props = newNode["props"];
  auto children = newNode["children"];

  if (!root || type != root->type) {
    root = createComponent(manager, type, props, children);
  }

  if (root->props != props) {
    root->updateProps(props);
  }

  auto minSize = std::min((size_t)children.size(), root->children.size());

  // update existing children
  for (int i = 0; i != minSize; ++i) {
    auto from = root->children.at(i);
    auto to = renderComponentTree(manager, from, children[i]);

    if (from != to) {
      root->replaceChild(from, to);
      deleteComponent(from);
    }
  }

  for (int i = minSize; i < children.size(); ++i) {
    auto child = children[i];
    auto component = createComponent(manager, child["type"].asString(),
                                     child["props"], child["children"]);

    root->appendChild(component);
  }

  while (root->children.size() > children.size()) {
    if (Component *pop = root->popChild())
      deleteComponent(pop);
  }

  return root;
}

QFlags<Qt::AlignmentFlag> parseQtAlignment(const std::string &s) {
  if (s == "top")
    return Qt::AlignTop;
  if (s == "left")
    return Qt::AlignLeft;
  if (s == "bottom")
    return Qt::AlignBottom;
  if (s == "right")
    return Qt::AlignRight;
  if (s == "center")
    return Qt::AlignCenter;
  if (s == "hcenter")
    return Qt::AlignHCenter;
  if (s == "vcenter")
    return Qt::AlignVCenter;

  return {};
}
