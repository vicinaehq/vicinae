#pragma once
#include "clipboard-history-command.hpp"
#include "extend/model-parser.hpp"
#include "extension/extension-grid-component.hpp"
#include "extension/extension-list-component.hpp"
#include "extension/extension-view.hpp"
#include <qtmetamacros.h>
#include <sys/un.h>

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

  virtual bool supportsSearch() const override { return false; }
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

  void executeAction(AbstractAction *action) override {
    qDebug() << "Execute action" << action->title();
    if (auto view = m_current) return static_cast<BaseView *>(view)->executeAction(action);
  }

  bool inputFilter(QKeyEvent *event) override {
    if (auto view = m_current) return view->inputFilter(event);

    return false;
  }

  void textChanged(const QString &text) override {
    if (auto view = m_current) view->textChanged(text);
  }

  bool supportsSearch() const override { return false; }

  ActionPanelV2Widget *actionPanel() const override { return nullptr; }

public:
  void render(const RenderModel &model) {
    if (m_index != model.index()) {
      auto view = std::visit(ViewVisitor(), model);

      connect(view, &ExtensionSimpleView::notificationRequested, this,
              &ExtensionViewWrapper::notificationRequested);

      if (auto previous = m_layout->widget(0)) {
        m_layout->removeWidget(previous);
        previous->deleteLater();
      }

      m_layout->addWidget(view);
      m_layout->setCurrentWidget(view);

      m_current = view;
      setActionPanelWidget(m_current->actionPanel());
      setSearchVisiblity(m_current->supportsSearch());
      setSearchAccessory(m_current->searchBarAccessory());
      m_current->initialize();
      m_current->activate();
      m_index = model.index();
    }

    m_current->render(model);
  }

  ExtensionViewWrapper() { setLayout(m_layout); }

signals:
  void notificationRequested(const QString &handler, const QJsonArray &args) const;
};
