#pragma once
#include "extension/extension-grid-component.hpp"
#include "extension/extension-list-component.hpp"
#include "extension/extension-view.hpp"
#include <qtmetamacros.h>

struct ViewVisitor {
  ExtensionSimpleView *operator()(const ListModel &model) const { return new ExtensionListComponent; }
  ExtensionSimpleView *operator()(const GridModel &model) const { return new ExtensionGridComponent; }
  ExtensionSimpleView *operator()(const FormModel &model) const { return nullptr; }
  ExtensionSimpleView *operator()(const InvalidModel &model) const { return nullptr; }
  ExtensionSimpleView *operator()(const RootDetailModel &model) const { return nullptr; }
};

class PlaceholderExtensionView : public ExtensionSimpleView {
public:
  PlaceholderExtensionView() {
    // m_topBar->input->hide();
    setupUI(new QWidget);
  }
  void render(const RenderModel &model) override {}
};

class ExtensionViewWrapper : public BaseView {
  Q_OBJECT
  ExtensionSimpleView *m_current = nullptr;
  QString m_searchText;
  QStackedLayout *m_layout = new QStackedLayout;
  int m_index = -1;

  void onDeactivate() override {
    if (m_current) m_current->deactivate();
  }
  void onActivate() override {
    if (m_current) m_current->activate();
  }

  void setSearchText(const QString &value) {
    if (m_current) {
      m_current->setSearchText(value);
      return;
    }

    m_searchText = value;
  }

  void setToast(const Toast *toast) override {
    if (m_current) m_current->setToast(toast);
  }

public:
  void render(const RenderModel &model) {
    if (m_index != model.index()) {
      auto view = std::visit(ViewVisitor(), model);

      connect(view, &ExtensionSimpleView::notificationRequested, this,
              &ExtensionViewWrapper::notificationRequested);

      if (auto previous = m_layout->widget(0)) {
        m_layout->replaceWidget(previous, view);
        previous->deleteLater();
        m_current = view;
        m_current->initialize();
        m_current->activate();

        if (m_searchText.isEmpty()) {
          m_current->setSearchText(m_searchText);
          m_searchText.clear();
        }

        m_index = model.index();
      }
    }

    m_current->render(model);
  }

  ExtensionViewWrapper() {
    m_layout->addWidget(new PlaceholderExtensionView);
    setLayout(m_layout);
  }

signals:
  void notificationRequested(const QString &handler, const QJsonArray &args) const;
};
