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
#include <qnamespace.h>
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
    return new ListItemComponent(manager, props, children,
                                 (QListWidget *)parent);
  if (type == "Label")
    return new LabelComponent(manager, props, children, parent);
  if (type == "Image")
    return new ImageComponent(props, children, parent);

  throw std::runtime_error("Tried to create unknown component of type " +
                           std::string(type));
}

static void deleteComponent(Component *component) {
  std::cout << "Deleting=" << component->type << std::endl;
  if (auto ui = component->ui)
    ui->deleteLater();
  delete component;
}

Component *renderComponentTree(ExtensionManager *manager, Component *root,
                               Json::Value newNode, Component *parent) {
  auto type = newNode["type"].asString();
  auto props = newNode["props"];
  auto children = newNode["children"];

  if (!root || type != root->type) {
    return createComponent(manager, type, props, children,
                           parent ? parent->ui : nullptr);
  }

  // if (root->props != props) {
  //}

  auto minSize = std::min((size_t)children.size(), root->children.size());

  // update existing children
  for (int i = 0; i != minSize; ++i) {
    auto from = root->children.at(i);
    auto to = renderComponentTree(manager, from, children[i], root);

    if (from != to) {
      std::cout << "[REPLACE] " << from->type << " to: " << to->type
                << std::endl;
      root->replaceChild(from, to);
      if (from->type != "ListItem")
        deleteComponent(from);
    }

    root->children[i] = to;
  }

  for (int i = minSize; i < children.size(); ++i) {
    auto child = children[i];
    std::cout << "new child " << child["type"] << " for parent of type "
              << (parent ? parent->type : "NO_PARENT") << std::endl;
    auto component =
        createComponent(manager, child["type"].asString(), child["props"],
                        child["children"], root->ui);

    root->appendChild(component);
  }

  while (root->children.size() > children.size()) {
    if (Component *pop = root->popChild()) {
      if (pop->type != "ListItem")
        deleteComponent(pop);
    }
  }

  root->updateProps(props);

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

Qt::ScrollBarPolicy parseQtScrollBarPolicy(const std::string &s) {
  if (s == "always-off")
    return Qt::ScrollBarPolicy::ScrollBarAlwaysOff;
  else if (s == "always-on")
    return Qt::ScrollBarPolicy::ScrollBarAlwaysOn;

  return Qt::ScrollBarPolicy::ScrollBarAsNeeded;
}
