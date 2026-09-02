#pragma once
#include <QKeyEvent>
#include <QUrl>
#include <QVariantList>
#include <QVariantMap>

#include "section-list-model.hpp"
#include "ui/views/base-view.hpp"

class ViewHostBase : public BaseView {
  Q_OBJECT

signals:
  void searchAccessoryUrlChanged();

public:
  virtual QUrl qmlComponentUrl() const = 0;
  virtual QUrl qmlSearchAccessoryUrl() const { return {}; }
  virtual QVariantMap qmlProperties() { return {}; }
  virtual void loadInitialData() {}
  virtual void onReactivated() {}

  void textChanged(const QString &) override {}
  bool inputFilter(QKeyEvent *event) override {
    if (event->isAutoRepeat() || event->modifiers() != Qt::ControlModifier) return false;

    int shortcutIndex = -1;
    if (event->key() >= Qt::Key_1 && event->key() <= Qt::Key_9) {
      shortcutIndex = event->key() - Qt::Key_1;
    } else if (event->key() == Qt::Key_0) {
      shortcutIndex = 9;
    }

    auto *model = quickAccessModel();
    return shortcutIndex >= 0 && model && model->activateQuickAccess(shortcutIndex);
  }

protected:
  virtual SectionListModel *quickAccessModel() { return nullptr; }
};

class FormViewBase : public ViewHostBase {
public:
  bool searchInteractive() const override { return false; }
};
