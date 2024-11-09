#pragma once
#include <iostream>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/value.h>
#include <qboxlayout.h>
#include <qbrush.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistwidget.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <string_view>

class ContainerComponent;

class Component {

public:
  std::string_view type;
  Json::Value props;
  std::vector<Component *> children;
  QWidget *ui = nullptr;

  Component(std::string_view name, Json::Value initialProps,
            Json::Value children)
      : type(name), props(initialProps) {}

  virtual void updateProps(Json::Value newProps) { props = newProps; }
  virtual void appendChild(Component *child) {}
  virtual void replaceChild(Component *prev, Component *next) {}
  virtual Component *popChild() { return nullptr; }
};

Component *createComponent(std::string_view type, Json::Value props,
                           Json::Value children);

Component *renderComponentTree(Component *root, Json::Value newNode);

class ListItemComponent : public Component {

public:
  QListWidgetItem *item;

  ListItemComponent(Json::Value props, Json::Value children)
      : Component("ListItem", props, children) {
    item = new QListWidgetItem();
    ui = (QWidget *)item;
    updateProps(props);
  }

  void updateProps(Json::Value newProps) override {
    item->setText(QString::fromStdString(newProps["label"].asString()));
    props = newProps;
  }
};

class ListComponent : public Component {
  QListWidget *list;

public:
  ListComponent(Json::Value props, Json::Value children)
      : Component("List", props, children) {
    list = new QListWidget();
    ui = list;

    for (const auto &child : children) {
      auto component = new ListItemComponent(child["props"], child["children"]);

      list->addItem(component->item);
      this->children.push_back(component);
    }
  };

  void updateProps(Json::Value newProps) override { props = newProps; };

  void appendChild(Component *child) override {
    if (auto *listItem = dynamic_cast<ListItemComponent *>(child)) {
      list->addItem(listItem->item);
    }
    children.push_back(child);
  }
};

class ContainerComponent : public Component {
  QBoxLayout *layout;

public:
  ContainerComponent(Json::Value props, Json::Value children)
      : Component("container", props, children) {
    layout = new QBoxLayout(QBoxLayout::Direction::TopToBottom);
    ui = new QWidget();

    ui->setLayout(layout);
    layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    updateProps(props);

    for (const auto &child : children) {
      auto component = createComponent(child["type"].asString(), child["props"],
                                       child["children"]);

      layout->addWidget(component->ui, 0, Qt::AlignTop);
      this->children.push_back(component);
    }
  }

  void updateProps(Json::Value newProps) override {
    if (auto el = newProps.find("direction"); el && el->isString()) {
      auto s = el->asString();

      if (s == "vertical")
        layout->setDirection(QBoxLayout::Direction::TopToBottom);
      else if (s == "horizontal")
        layout->setDirection(QBoxLayout::Direction::LeftToRight);
    }

    if (auto margins = props.find("margins")) {
      if (margins->isArray() && margins->size() == 4) {
        layout->setContentsMargins(
            (*margins)[0].asUInt64(), (*margins)[1].asUInt64(),
            (*margins)[2].asUInt64(), (*margins)[3].asUInt64());
      } else if (margins->isInt()) {
        auto n = margins->asInt();

        layout->setContentsMargins(n, n, n, n);
      }
    } else {
      layout->setContentsMargins(0, 0, 0, 0);
    }

    props = newProps;
  }

  void appendChild(Component *child) override {
    layout->addWidget(child->ui);
    children.push_back(child);
  }

  void replaceChild(Component *prev, Component *to) override {
    layout->replaceWidget(prev->ui, to->ui);
  }

  Component *popChild() override {
    if (children.empty())
      return nullptr;

    auto lastPos = children.size() - 1;
    auto component = children.at(lastPos);

    layout->takeAt(lastPos);
    children.pop_back();

    return component;
  }
};

class SearchInputComponent : public Component {
  QLineEdit *input;

public:
  SearchInputComponent(Json::Value props, Json::Value children)
      : Component("SearchInput", props, children) {
    input = new QLineEdit();
    ui = input;

    updateProps(props);
  }

  void updateProps(Json::Value newProps) override {
    if (auto el = newProps.find("placeholder"); el && el->isString()) {
      input->setPlaceholderText(QString::fromStdString(el->asString()));
    }

    if (auto el = newProps.find("value"); el && el->isString()) {
      input->setText(QString::fromStdString(el->asString()));
    }

    if (auto el = newProps.find("style"); el && el->isString()) {
      input->setStyleSheet(QString::fromStdString(el->asString()));
    }

    props = newProps;
  }
};

class ImageComponent : public Component {
  QLabel *label;

public:
  ImageComponent(Json::Value props, Json::Value children)
      : Component("Image", props, children) {
    label = new QLabel();
    ui = label;
  }

  void updateProps(Json::Value newProps) override {
    QPixmap map;

    if (auto el = newProps.get("path", ""); !el.empty()) {

      map.load(QString::fromStdString(el.asString()));
    }

    if (newProps.isMember("width")) {
      map = map.scaledToWidth(newProps["width"].asInt(),
                              Qt::SmoothTransformation);
    }

    if (auto el = newProps.isMember("height")) {
      map = map.scaledToHeight(newProps["height"].asInt(),
                               Qt::SmoothTransformation);
    }

    label->setPixmap(map);
    props = newProps;
  }
};
