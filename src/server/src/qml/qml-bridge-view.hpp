#pragma once
#include "qml-command-list-model.hpp"
#include "ui/views/base-view.hpp"

class QmlBridgeViewBase : public BaseView {
public:
  virtual QmlCommandListModel *qmlModel() const = 0;

  // Called by QmlLauncherWindow after the QML context property is set,
  // so that modelReset reaches the QML Connections handler.
  virtual void loadInitialData() = 0;

  void textChanged(const QString &text) override {
    if (auto *m = qmlModel()) m->setFilter(text);
  }

  bool inputFilter(QKeyEvent *) override { return false; }
};

template <typename ModelType> class QmlBridgeView : public QmlBridgeViewBase {
  ModelType *m_model = nullptr;

public:
  QmlCommandListModel *qmlModel() const override { return m_model; }

  void initialize() override {
    m_model = new ModelType(this);
    m_model->initialize(context());
    setSearchPlaceholderText(m_model->searchPlaceholder());
  }

  void loadInitialData() override { m_model->setFilter(searchText()); }
};
