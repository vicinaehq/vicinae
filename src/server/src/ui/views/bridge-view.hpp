#pragma once
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

    const auto key = Keyboard::resolveKey(static_cast<Qt::Key>(event->key()), event->nativeScanCode());
    int shortcutIndex = -1;
    if (key >= Qt::Key_1 && key <= Qt::Key_9) {
      shortcutIndex = key - Qt::Key_1;
    } else if (key == Qt::Key_0) {
      shortcutIndex = 9;
    }

    auto *model = quickAccessModel();
    return shortcutIndex >= 0 && model && model->activateQuickAccess(shortcutIndex);
  }

protected:
  virtual SectionListModel *quickAccessModel() { return listModel(); }
};

class FormViewBase : public ViewHostBase {
  Q_OBJECT

public:
  bool searchInteractive() const override { return false; }
};
