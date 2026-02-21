#pragma once
#include "command-list-model.hpp"
#include "ui/views/base-view.hpp"
#include <QUrl>
#include <QVariantMap>

/// General-purpose bridge: any QML component can be pushed as a view.
class ViewHostBase : public BaseView {
  Q_OBJECT

signals:
  void searchAccessoryUrlChanged();

public:
  /// The QML component to instantiate (e.g. "qrc:/Vicinae/CommandListView.qml").
  virtual QUrl qmlComponentUrl() const = 0;

  /// Optional QML component to load in the SearchBar as an accessory (e.g. filter dropdown).
  virtual QUrl qmlSearchAccessoryUrl() const { return {}; }

  /// Properties passed to the QML component at instantiation.
  virtual QVariantMap qmlProperties() const { return {}; }

  /// Called after the QML component is pushed and properties set.
  virtual void loadInitialData() {}

  /// Called when popping back to this view (component preserved by StackView).
  virtual void onReactivated() {}

  void textChanged(const QString &) override {}
  bool inputFilter(QKeyEvent *) override { return false; }
};

/// Bridge for form views — disables the search bar input.
class FormViewBase : public ViewHostBase {
public:
  bool searchInteractive() const override { return false; }
};

/// Convenience bridge for views backed by a CommandListModel.
template <typename ModelType> class BridgeView : public ViewHostBase {
  ModelType *m_model = nullptr;

public:
  QUrl qmlComponentUrl() const override { return m_model->qmlComponentUrl(); }

  QVariantMap qmlProperties() const override {
    return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
  }

  void initialize() override {
    m_model = new ModelType(this);
    m_model->initialize(context());
    setSearchPlaceholderText(m_model->searchPlaceholder());
  }

  void textChanged(const QString &text) override { m_model->setFilter(text); }

  void beforePop() override { m_model->beforePop(); }

  void onReactivated() override { m_model->refreshActionPanel(); }

  void loadInitialData() override { m_model->setFilter(searchText()); }
};
