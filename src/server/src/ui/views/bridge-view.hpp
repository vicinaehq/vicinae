#pragma once
#include <QUrl>
#include <QVariantList>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>
#include "ui/views/base-view.hpp"

class SectionListModel;

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
  bool inputFilter(QKeyEvent *event) override;
};

class FormViewBase : public ViewHostBase {
  Q_OBJECT

public:
  bool searchInteractive() const override { return false; }
};
