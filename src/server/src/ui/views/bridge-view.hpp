#pragma once
#include <QUrl>
#include <QVariantList>
#include <QVariantMap>
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
  bool inputFilter(QKeyEvent *) override { return false; }
};

class FormViewBase : public ViewHostBase {
public:
  bool searchInteractive() const override { return false; }
};
