#pragma once
#include "command-list-model.hpp"
#include "ui/views/base-view.hpp"
#include <QUrl>
#include <QVariantMap>

class ViewHostBase : public BaseView {
  Q_OBJECT

signals:
  void searchAccessoryUrlChanged();

public:
  virtual QUrl qmlComponentUrl() const = 0;
  virtual QUrl qmlSearchAccessoryUrl() const { return {}; }
  virtual QVariantMap qmlProperties() const { return {}; }
  virtual void loadInitialData() {}
  virtual void onReactivated() {}

  void textChanged(const QString &) override {}
  bool inputFilter(QKeyEvent *) override { return false; }
};

class FormViewBase : public ViewHostBase {
public:
  bool searchInteractive() const override { return false; }
};

template <typename ModelType> class BridgeView : public ViewHostBase {
  ModelType *m_model = nullptr;

public:
  QUrl qmlComponentUrl() const override { return m_model->qmlComponentUrl(); }

  QVariantMap qmlProperties() const override {
    return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
  }

  void initialize() override {
    m_model = new ModelType(this);
    m_model->setScope(ViewScope(context(), this));
    m_model->initialize();
    setSearchPlaceholderText(m_model->searchPlaceholder());
  }

  void textChanged(const QString &text) override { m_model->setFilter(text); }

  void beforePop() override { m_model->beforePop(); }

  void onReactivated() override { m_model->refreshActionPanel(); }

  void loadInitialData() override { m_model->setFilter(searchText()); }
};
