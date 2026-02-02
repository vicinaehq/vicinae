#pragma once
#include <qlogging.h>
#include <qtmetamacros.h>
#include "extend/extension-detail-view.hpp"
#include "extension/extension-command-controller.hpp"
#include "extension/extension-form-component.hpp"
#include "extension/extension-grid-component.hpp"
#include "extension/extension-list-component.hpp"
#include "extension/extension-view.hpp"

struct ViewVisitor {
  ExtensionSimpleView *operator()(const ListModel &model) const { return new ExtensionListComponent; }
  ExtensionSimpleView *operator()(const GridModel &model) const { return new ExtensionGridComponent; }
  ExtensionSimpleView *operator()(const FormModel &model) const { return new ExtensionFormComponent; }
  ExtensionSimpleView *operator()(const RootDetailModel &model) const { return new ExtensionDetailView; }
  ExtensionSimpleView *operator()(const InvalidModel &model) const { return nullptr; }
};

class ExtensionViewWrapper : public BaseView {
  Q_OBJECT
  ExtensionSimpleView *m_current = nullptr;
  QString m_searchText;
  QStackedLayout *m_layout = new QStackedLayout;
  ExtensionCommandController *m_controller;
  int m_index = -1;

  void onDeactivate() override {
    if (m_current) m_current->deactivate();
  }
  void onActivate() override {
    if (m_current) { m_current->activate(); }
  }

  bool inputFilter(QKeyEvent *event) override {
    if (auto view = m_current) return view->inputFilter(event);

    return false;
  }

  void textChanged(const QString &text) override {
    if (auto view = m_current) view->textChanged(text);
  }

  bool supportsSearch() const override { return m_current ? m_current->supportsSearch() : false; }

  void initialize() override { setLoading(true); }

public:
  void render(const RenderModel &model) {
    if (m_index != model.index()) {
      auto view = std::visit(ViewVisitor(), model);

      if (auto previous = m_layout->widget(0)) {
        m_layout->removeWidget(previous);
        previous->deleteLater();
        m_current = nullptr;
      }

      if (!view) return;

      connect(view, &ExtensionSimpleView::notificationRequested, this,
              [this](const QString &handlerId, const QJsonArray &values) {
                m_controller->notify(handlerId, values);
              });

      m_layout->addWidget(view);
      m_layout->setCurrentWidget(view);

      m_current = view;
      m_current->setProxy(this);
      m_current->setExtensionCommandController(m_controller);

      setLoading(false);
      setTopBarVisiblity(m_current->needsGlobalTopBar());
      setSearchVisibility(m_current->supportsSearch());
      setStatusBarVisiblity(m_current->needsGlobalStatusBar());
      setSearchPlaceholderText("");
      context()->navigation->clearActions(this);

      if (auto accessory = m_current->searchBarAccessory()) {
        setSearchAccessory(accessory);
      } else {
        clearSearchAccessory();
      }

      m_current->initialize();
      m_current->activate();

      m_index = model.index();

      auto text = searchText();

      m_current->render(model);

      if (!text.isEmpty()) { view->textChanged(text); }
      return;
    }

    m_current->render(model);
  }

  ExtensionViewWrapper(ExtensionCommandController *controller) : m_controller(controller) {
    setLayout(m_layout);
  }

signals:
  void notificationRequested(const QString &handler, const QJsonArray &args) const;
};
