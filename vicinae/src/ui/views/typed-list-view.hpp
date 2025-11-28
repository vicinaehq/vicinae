#pragma once
#include "keyboard/keyboard.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/keybinding/keybinding-service.hpp"
#include "simple-view.hpp"
#include "ui/empty-view/empty-view.hpp"
#include "services/config/config-service.hpp"
#include "navigation-controller.hpp"
#include "ui/form/selector-input.hpp"
#include "ui/search-bar/search-bar.hpp"
#include <absl/strings/str_format.h>
#include <qlogging.h>
#include <qtconcurrentfilter.h>
#include <qwidget.h>
#include "ui/split-detail/split-detail.hpp"
#include "ui/vlist/vlist.hpp"

template <typename ModelType> class TypedListView : public SimpleView {
public:
  using ItemType = typename ModelType::Item;

  TypedListView(QWidget *parent = nullptr) {}

  virtual ~TypedListView() = default;

  void setModel(ModelType *model) {
    m_model = model;
    m_list->setModel(model);
  }

  ModelType *model() const { return m_model; }

protected:
  virtual QWidget *generateDetail(const ItemType &item) const { return nullptr; }
  virtual std::unique_ptr<CompleterData> createCompleter(const ItemType &item) const { return nullptr; }
  virtual std::unique_ptr<ActionPanelState> createActionPanel(const ItemType &item) const { return nullptr; }

  virtual void itemSelected() {}

  virtual void emptied() {}

  virtual bool inputFilter(QKeyEvent *event) override {
    auto config = ServiceRegistry::instance()->config();
    const QString &keybinding = config->value().keybinding;

    if (event->modifiers() == Qt::ControlModifier) {
      if (KeyBindingService::isDownKey(event, keybinding)) { return m_list->selectDown(); }
      if (KeyBindingService::isUpKey(event, keybinding)) { return m_list->selectUp(); }
      if (KeyBindingService::isLeftKey(event, keybinding)) {
        context()->navigation->popCurrentView();
        return true;
      }
      if (KeyBindingService::isRightKey(event, keybinding)) {
        m_list->activateCurrentSelection();
        return true;
      }
    }

    if (Keyboard::Shortcut(Keybind::OpenSearchAccessorySelector) == event) {
      // FIXME: improve this so that we don't need dynamic cast
      if (auto accessory = dynamic_cast<SelectorInput *>(currentSearchAccessory())) {
        accessory->openSelector();
      }
    }

    if (event->modifiers().toInt() == 0) {
      switch (event->key()) {
      case Qt::Key_Up:
        return m_list->selectUp();
        break;
      case Qt::Key_Down:
        return m_list->selectDown();
        break;
      case Qt::Key_Tab: {
        if (!context()->navigation->hasCompleter()) {
          // m_list->selectNext();
          return true;
        }
        break;
      }
      case Qt::Key_Return:
      case Qt::Key_Enter:
        m_list->activateCurrentSelection();
        return true;
      }
    }

    return SimpleView::inputFilter(event);
  }

  void forceReselection() {}

  virtual void initialize() override { setupUI(); }

  virtual void selectionChanged(std::optional<typename ModelType::Index> idx) {
    if (!idx || !m_model) {
      destroyCompleter();
      clearActions();
      m_split->setDetailVisibility(false);

      if (m_model && m_model->isEmpty()) { m_content->setCurrentWidget(m_emptyView); }

      return;
    }

    if (m_content->currentWidget() != m_split) { m_content->setCurrentWidget(m_split); }

    std::optional<ItemType> item = m_model->fromIndex(idx.value());

    if (!item) {
      qWarning() << "Invalid data at index" << idx.value();
      return;
    }

    setActions(createActionPanel(item.value()));

    if (auto completer = createCompleter(item.value())) {
      activateCompleter(completer->arguments, completer->iconUrl);
    } else {
      destroyCompleter();
    }

    auto detail = generateDetail(item.value());

    if (detail) { setDetail(detail); }
    m_split->setDetailVisibility(detail);
  }

  virtual void itemActivated(typename ModelType::Index idx) { executePrimaryAction(); }

  QWidget *detail() const { return m_split->detailWidget(); }

  void setDetail(QWidget *widget) { m_split->setDetailWidget(widget); }

  /**
   * Wrap the list view UI inside a layout if necessary
   */
  virtual QWidget *wrapUI(QWidget *content) { return content; }

  void setupUI() {
    m_split = new SplitDetailWidget(this);
    m_content = new QStackedWidget(this);
    m_emptyView = new EmptyViewWidget(this);
    m_list = new vicinae::ui::VListWidget;
    m_content->addWidget(m_split);
    m_content->addWidget(m_emptyView);
    m_content->setCurrentWidget(m_split);

    m_emptyView->setTitle("No results");
    m_emptyView->setDescription("No results matching your search. You can try to refine your search.");
    m_emptyView->setIcon(ImageURL::builtin("magnifying-glass"));

    m_split->setMainWidget(m_list);

    connect(m_list, &vicinae::ui::VListWidget::itemSelected, this, &TypedListView::selectionChanged);
    connect(m_list, &vicinae::ui::VListWidget::itemActivated, this, &TypedListView::itemActivated);
    /*
connect(m_list, &OmniList::virtualHeightChanged, this, [this](int height) {
  if (m_list->items().empty() && (!searchText().isEmpty() || !isLoading())) {
    // ui->destroyCompleter();
    m_content->setCurrentWidget(m_emptyView);
    return;
  }

  m_content->setCurrentWidget(m_split);
});
    */

    SimpleView::setupUI(wrapUI(m_content));
  }

  vicinae::ui::VListWidget *m_list;
  ModelType *m_model = nullptr;
  SplitDetailWidget *m_split;
  QStackedWidget *m_content;
  EmptyViewWidget *m_emptyView;
};
