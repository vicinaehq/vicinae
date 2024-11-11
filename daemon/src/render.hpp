#pragma once
#include "extension_manager.hpp"
#include <iostream>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/value.h>
#include <qboxlayout.h>
#include <qbrush.h>
#include <qflags.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistwidget.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qsizepolicy.h>
#include <qwidget.h>
#include <stdexcept>
#include <string_view>

QFlags<Qt::AlignmentFlag> parseQtAlignment(const std::string &s);

class Component {

public:
  std::string_view type;
  Json::Value props;
  std::vector<Component *> children;
  QWidget *ui = nullptr;
  QWidget *parent = nullptr;

  Component(std::string_view name, Json::Value initialProps,
            Json::Value children, QWidget *parent = nullptr)
      : type(name), props(initialProps), parent(parent) {}

  virtual void updateProps(Json::Value newProps) {}
  void updateInitialProps(QWidget *widget, Json::Value newProps) {
    auto parent = widget->parentWidget();

    if (!newProps.find("selfAlign")) {
      return;
    }

    std::cout << "parent2=" << parent << std::endl;

    if (parent) {
      if (parent->layout()) {
        if (auto layout = qobject_cast<QBoxLayout *>(parent->layout())) {
          std::cout << "has layout!" << std::endl;
          if (auto align = newProps.find("selfAlign");
              align && align->isString()) {

            std::cout << ">>>>>>> ALIGN <<<<<<" << std::endl;

            for (int i = 0; i != layout->count(); ++i) {
              auto item = layout->itemAt(i);
              auto w = item->widget();

              if (w != widget) {
                std::cout << "Not right widget" << std::endl;
                continue;
              }

              item->setAlignment(parseQtAlignment(align->asString()));
              std::cout << "set layout align for " << type << std::endl;
            }
          }
          if (auto stretch = newProps.find("stretch");
              stretch && stretch->isUInt64()) {
            std::cout << "set stretch factor for " << type << " to "
                      << stretch->asUInt64() << std::endl;
            layout->setStretchFactor(widget, stretch->asUInt64());
          }
        } else {
          std::cout << "Parent is not a QBoxLayout*" << this->type << std::endl;
        }
      } else {
        std::cout << "parent has no layout " << this->type << std::endl;
      }
    } else {
      std::cout << "No parent for widget " << this->type << std::endl;
    }

    props = newProps;
  }
  virtual void appendChild(Component *child) {}
  virtual void replaceChild(Component *prev, Component *next) {}
  virtual Component *popChild() { return nullptr; }
};

static QSize computeTreeSizeHint(Component *root) {
  int width = 0, height = 0;
  std::stack<Component *> stack;

  stack.push(root);

  while (!stack.empty()) {
    auto component = stack.top();
    stack.pop();

    auto size = component->ui->sizeHint();

    std::cout << "COMPUTE FOR TYPE=" << component->type << " " << size.width()
              << "x" << size.height() << std::endl;

    if (size.width() > width)
      width = size.width();
    if (size.height() > height)
      height = size.height();

    for (const auto &child : component->children) {
      stack.push(child);
    }
  }

  return QSize{width, height};
}

Component *createComponent(ExtensionManager *manager, std::string_view type,
                           Json::Value props, Json::Value children,
                           QWidget *parent = nullptr);

Component *renderComponentTree(ExtensionManager *manager, Component *root,
                               Json::Value newNode);

class ListItemComponent : public Component {

public:
  QListWidgetItem *item;
  ExtensionManager *manager;

  ListItemComponent(ExtensionManager *manager, Json::Value props,
                    Json::Value children, QListWidget *list = nullptr)
      : Component("ListItem", props, children), manager(manager) {
    item = new QListWidgetItem(list);
    ui = (QWidget *)item;
    updateProps(props);

    if (children.size() > 0 && list) {
      auto child = children[0];
      std::cout << "before list" << std::endl;
      Component *component =
          createComponent(manager, child["type"].asString(), child["props"],
                          child["children"], list);
      std::cout << "after list" << std::endl;

      list->setItemWidget(item, component->ui);
      std::cout << "list size=" << component->ui->sizeHint().width() << "x"
                << component->ui->sizeHint().height() << std::endl;
      auto qsize = computeTreeSizeHint(component);

      std::cout << "w=" << qsize.width() << " h=" << qsize.height()
                << std::endl;

      item->setSizeHint(component->ui->sizeHint());
      this->children.push_back(component);
    }
  }

  void updateProps(Json::Value newProps) override {
    if (auto selected = newProps.find("selected");
        selected && selected->isBool()) {
      std::cout << "set selected item=" << selected->asBool() << std::endl;
      item->setSelected(selected->asBool());
    }

    if (auto label = newProps.find("label"); label && label->isString()) {
      item->setText(QString::fromStdString(label->asString()));
    }

    props = newProps;
  }
};

class ListComponent : public Component {
  QListWidget *list;
  ExtensionManager *manager;

public:
  ListComponent(ExtensionManager *manager, Json::Value props,
                Json::Value children, QWidget *parent = nullptr)
      : Component("List", props, children), manager(manager) {
    list = new QListWidget(parent);
    ui = list;

    for (const auto &child : children) {
      auto component = new ListItemComponent(manager, child["props"],
                                             child["children"], list);

      list->addItem(component->item);
      this->children.push_back(component);
    }

    if (auto currentRowChanged = props.find("currentRowChanged");
        currentRowChanged && currentRowChanged->isString()) {
      std::string id = currentRowChanged->asString();

      list->connect(list, &QListWidget::currentRowChanged, [this, id](int row) {
        this->manager->handler("currentRowChanged", id, row);
      });
    }

    updateProps(props);
  };

  void updateProps(Json::Value newProps) override {
    Component::updateInitialProps(ui, newProps);

    if (auto spacing = newProps.find("spacing");
        spacing && spacing->isUInt64()) {
      list->setSpacing(spacing->asUInt64());
    }

    if (auto align = newProps.find("alignItems"); align && align->isString()) {
      list->setItemAlignment(parseQtAlignment(align->asString()));
    }

    if (auto selected = newProps.find("selected");
        selected && selected->isInt()) {
      std::cout << "set selected item=" << selected->asInt() << std::endl;
      list->setCurrentRow(selected->asInt());
    }

    props = newProps;
  };

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
  ContainerComponent(ExtensionManager *manager, Json::Value props,
                     Json::Value children, QWidget *parent = nullptr)
      : Component("container", props, children) {
    layout = new QBoxLayout(QBoxLayout::Direction::TopToBottom);
    ui = new QWidget(parent);
    ui->setLayout(layout);

    for (const auto &child : children) {
      auto component = createComponent(manager, child["type"].asString(),
                                       child["props"], child["children"], ui);

      layout->addWidget(component->ui);
      component->updateProps(child["props"]);
      this->children.push_back(component);
    }

    updateProps(props);
  }

  void updateProps(Json::Value newProps) override {
    Component::updateInitialProps(ui, newProps);

    auto item = layout->itemAt(0);

    if (auto el = newProps.find("style"); el && el->isString()) {
      ui->setStyleSheet(QString::fromStdString(el->asString()));
    }

    if (auto el = newProps.find("direction"); el && el->isString()) {
      auto s = el->asString();

      if (s == "vertical")
        layout->setDirection(QBoxLayout::Direction::TopToBottom);
      else if (s == "horizontal")
        layout->setDirection(QBoxLayout::Direction::LeftToRight);
    }

    if (auto el = newProps.find("align"); el && el->isString()) {
      layout->setAlignment(parseQtAlignment(el->asString()));
    }

    if (auto el = newProps.find("spacing"); el && el->isUInt64()) {
      layout->setSpacing(el->asUInt64());
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
  ExtensionManager *manager;

public:
  SearchInputComponent(ExtensionManager *manager, Json::Value props,
                       Json::Value children)
      : Component("SearchInput", props, children), manager(manager) {
    input = new QLineEdit();
    ui = input;

    if (auto el = props.find("onTextChanged"); el && el->isString()) {
      std::string handlerId = el->asString();

      input->connect(input, &QLineEdit::textChanged,
                     [this, handlerId](const QString &s) {
                       this->manager->handler<std::string>(
                           "onTextChanged", handlerId, s.toStdString());
                     });
    }

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
  ImageComponent(Json::Value props, Json::Value children,
                 QWidget *parent = nullptr)
      : Component("Image", props, children) {
    label = new QLabel(parent);
    ui = label;
    updateProps(props);
  }

  void updateProps(Json::Value newProps) {
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
    std::cout << label->sizeHint().width() << "x" << label->sizeHint().height()
              << std::endl;
    props = newProps;
  }
};

class LabelComponent : public Component {
  QLabel *label;
  ExtensionManager *manager;

public:
  LabelComponent(ExtensionManager *manager, Json::Value props,
                 Json::Value children, QWidget *parent = nullptr)
      : Component("Label", props, children), manager(manager) {
    std::cout << "parent=" << parent << std::endl;
    label = new QLabel(parent);
    ui = label;

    updateProps(props);
  }

  void updateProps(Json::Value newProps) override {
    Component::updateInitialProps(ui, newProps);

    if (auto el = props.find("text"); el && el->isString()) {
      label->setText(QString::fromStdString(el->asString()));
    }

    if (auto el = newProps.find("style"); el && el->isString()) {
      label->setStyleSheet(QString::fromStdString(el->asString()));
    }

    props = newProps;
  }
};
