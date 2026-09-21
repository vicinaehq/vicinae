#pragma once
#include <algorithm>
#include <QKeyEvent>
#include <QUrl>
#include <QVariantList>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>

#include "keyboard/keyboard.hpp"
#include "section-list-model.hpp"
#include "ui/views/base-view.hpp"

class ViewHostBase : public BaseView {
  Q_OBJECT
  QML_NAMED_ELEMENT(ViewHostBase)
  QML_UNCREATABLE("")
  Q_PROPERTY(SectionListModel *listModel READ listModel CONSTANT)

signals:
  void searchAccessoryUrlChanged();

public:
  virtual QUrl qmlComponentUrl() const = 0;
  virtual SectionListModel *listModel() const { return nullptr; }
  virtual QUrl qmlSearchAccessoryUrl() const { return {}; }
  virtual QVariantMap qmlProperties() { return {}; }
  virtual void loadInitialData() {}
  virtual void onReactivated() {}

  void textChanged(const QString &) override {}
  bool inputFilter(QKeyEvent *event) override {
    if (event->type() != QEvent::KeyPress || event->isAutoRepeat() ||
        (event->modifiers() & ~Qt::KeypadModifier) != Qt::ControlModifier) {
      return false;
    }

    const Keyboard::KeyPress press(*event);
    const auto candidates = press.candidates();
    const auto digitKey = std::ranges::find_if(
        candidates, [](const Keyboard::Shortcut &c) { return c.key() >= Qt::Key_0 && c.key() <= Qt::Key_9; });
    if (digitKey == candidates.end()) return false;

    const int digit = digitKey->key() - Qt::Key_0;
    const int shortcutIndex = digit == 0 ? 9 : digit - 1;
    auto *model = quickAccessModel();
    return model && model->activateQuickAccess(shortcutIndex);
  }

protected:
  virtual SectionListModel *quickAccessModel() { return listModel(); }
};

class FormViewBase : public ViewHostBase {
  Q_OBJECT

public:
  bool searchInteractive() const override { return false; }
};
