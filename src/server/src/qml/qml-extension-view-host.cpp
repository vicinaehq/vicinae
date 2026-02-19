#include "qml-extension-view-host.hpp"
#include "navigation-controller.hpp"
#include "qml-utils.hpp"
#include <chrono>

static const std::chrono::milliseconds THROTTLE_DEBOUNCE_DURATION(300);

QmlExtensionViewHost::QmlExtensionViewHost(ExtensionCommandController *controller, QObject *parent)
    : QmlBridgeViewBase(), m_controller(controller), m_searchDebounce(new QTimer(this)) {
  m_searchDebounce->setSingleShot(true);
  connect(m_searchDebounce, &QTimer::timeout, this, &QmlExtensionViewHost::handleDebouncedSearch);
}

QUrl QmlExtensionViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/ExtensionView.qml"));
}

QUrl QmlExtensionViewHost::qmlSearchAccessoryUrl() const {
  if (m_linkAccessoryText.isEmpty()) return {};
  return QUrl(QStringLiteral("qrc:/Vicinae/FormLinkAccessory.qml"));
}

QVariantMap QmlExtensionViewHost::qmlProperties() const {
  return {{QStringLiteral("host"), QVariant::fromValue(const_cast<QmlExtensionViewHost *>(this))}};
}

void QmlExtensionViewHost::loadInitialData() {
  setLoading(true);
  setSearchInteractive(false);
}

void QmlExtensionViewHost::onReactivated() {
  if (m_listModel) {
    m_listModel->refreshActionPanel();
  } else if (m_gridModel) {
    m_gridModel->refreshActionPanel();
  } else if (m_viewType == "detail" && m_detailActions) {
    auto notify = [this](const QString &handler, const QJsonArray &args) {
      notifyExtension(handler, args);
    };
    setActions(ExtensionActionPanelBuilder::build(*m_detailActions, notify, &m_submenuCache));
  } else if (m_viewType == "form" && m_formActions) {
    auto notify = [this](const QString &handler, const QJsonArray &args) {
      notifyExtension(handler, args);
    };
    auto submit = [this]() -> std::expected<QJsonObject, QString> { return m_formModel->submit(); };
    setActions(ExtensionActionPanelBuilder::build(*m_formActions, notify, &m_submenuCache,
                                                  ActionPanelState::ShortcutPreset::Form, submit));
  }
}

void QmlExtensionViewHost::render(const RenderModel &model) {
  if (m_firstRender) {
    m_firstRender = false;
    handleFirstRender(model);
    return;
  }

  if (auto *listModel = std::get_if<ListModel>(&model)) {
    if (m_listModel) {
      renderList(*listModel);
    } else {
      // View type changed or unexpected — fallback
      emit fallbackRequired(model);
    }
  } else if (auto *gridModel = std::get_if<GridModel>(&model)) {
    if (m_gridModel) {
      renderGrid(*gridModel);
    } else {
      emit fallbackRequired(model);
    }
  } else if (auto *detailModel = std::get_if<RootDetailModel>(&model)) {
    if (m_viewType == "detail") {
      renderDetail(*detailModel);
    } else {
      emit fallbackRequired(model);
    }
  } else if (auto *formModel = std::get_if<FormModel>(&model)) {
    if (m_formModel) {
      renderForm(*formModel);
    } else {
      emit fallbackRequired(model);
    }
  } else {
    emit fallbackRequired(model);
  }
}

void QmlExtensionViewHost::handleFirstRender(const RenderModel &model) {
  if (auto *listModel = std::get_if<ListModel>(&model)) {
    auto notify = [this](const QString &handler, const QJsonArray &args) {
      notifyExtension(handler, args);
    };

    m_listModel = new QmlExtensionListModel(notify, this);
    m_listModel->initialize(context());
    m_viewType = "list";
    setSearchInteractive(true);
    renderList(*listModel);

    emit viewTypeChanged();
    emit contentModelChanged();
  } else if (auto *gridModel = std::get_if<GridModel>(&model)) {
    auto notify = [this](const QString &handler, const QJsonArray &args) {
      notifyExtension(handler, args);
    };

    m_gridModel = new QmlExtensionGridModel(notify, this);
    m_gridModel->initialize(context());
    m_viewType = "grid";
    setSearchInteractive(true);
    renderGrid(*gridModel);

    emit viewTypeChanged();
    emit contentModelChanged();
  } else if (auto *detailModel = std::get_if<RootDetailModel>(&model)) {
    m_viewType = "detail";
    setSearchInteractive(false);
    renderDetail(*detailModel);
    emit viewTypeChanged();
  } else if (auto *formModel = std::get_if<FormModel>(&model)) {
    auto notify = [this](const QString &handler, const QJsonArray &args) {
      notifyExtension(handler, args);
    };
    m_formModel = new QmlExtensionFormModel(notify, this);
    m_viewType = "form";
    setSearchInteractive(false);
    renderForm(*formModel);

    emit viewTypeChanged();
    emit formModelChanged();
  } else {
    emit fallbackRequired(model);
  }
}

void QmlExtensionViewHost::renderList(const ListModel &model) {
  // Update search-related state
  m_onSearchTextChange = model.onSearchTextChange;
  m_filtering = model.filtering;

  if (model.throttle != m_throttle) {
    m_throttle = model.throttle;
    m_searchDebounce->stop();
    m_searchDebounce->setInterval(m_throttle ? THROTTLE_DEBOUNCE_DURATION : std::chrono::milliseconds(0));
  }

  if (!model.navigationTitle.empty()) { setNavigationTitle(model.navigationTitle.c_str()); }
  if (!model.searchPlaceholderText.empty()) { setSearchPlaceholderText(model.searchPlaceholderText.c_str()); }
  if (auto text = model.searchText) { setSearchText(text->c_str()); }

  bool wasLoading = m_isLoading;
  m_isLoading = model.isLoading;
  if (wasLoading != m_isLoading) { emit isLoadingChanged(); emit suppressEmptyViewChanged(); }
  setLoading(model.isLoading);

  if (model.dirty) {
    m_selectFirstOnReset = m_shouldResetSelection;
    emit selectFirstOnResetChanged();
    m_listModel->setExtensionData(model, m_shouldResetSelection);
    m_selectFirstOnReset = true;
    emit selectFirstOnResetChanged();
    m_shouldResetSelection = false;
  }
}

void QmlExtensionViewHost::renderGrid(const GridModel &model) {
  m_onSearchTextChange = model.onSearchTextChange;
  m_filtering = model.filtering;

  if (model.throttle != m_throttle) {
    m_throttle = model.throttle;
    m_searchDebounce->stop();
    m_searchDebounce->setInterval(m_throttle ? THROTTLE_DEBOUNCE_DURATION : std::chrono::milliseconds(0));
  }

  if (!model.navigationTitle.empty()) { setNavigationTitle(model.navigationTitle.c_str()); }
  if (!model.searchPlaceholderText.empty()) { setSearchPlaceholderText(model.searchPlaceholderText.c_str()); }
  if (auto text = model.searchText) { setSearchText(text->c_str()); }

  bool wasLoading = m_isLoading;
  m_isLoading = model.isLoading;
  if (wasLoading != m_isLoading) { emit isLoadingChanged(); emit suppressEmptyViewChanged(); }
  setLoading(model.isLoading);

  if (model.dirty) { m_gridModel->setExtensionData(model); }
}

void QmlExtensionViewHost::textChanged(const QString &text) {
  bool had = m_hasSearchText;
  m_hasSearchText = !text.isEmpty();
  if (had != m_hasSearchText) emit suppressEmptyViewChanged();

  if (m_throttle) {
    m_searchDebounce->start();
  } else {
    handleDebouncedSearch();
  }
}

void QmlExtensionViewHost::handleDebouncedSearch() {
  auto text = searchText();

  // Client-side filtering
  if (m_listModel) {
    m_listModel->setFilter(text);
  } else if (m_gridModel) {
    m_gridModel->setFilter(text);
  }

  // Notify extension of search text change
  if (m_onSearchTextChange) {
    m_shouldResetSelection = !m_filtering;
    notifyExtension(m_onSearchTextChange->c_str(), {text});
  }
}

bool QmlExtensionViewHost::inputFilter(QKeyEvent *event) {
  // Grid views need arrow key handling — the QML side handles this via
  // GenericGridView's focus handling, so we don't need to intercept here.
  // List views are handled by GenericListView's focus handling.
  return false;
}

void QmlExtensionViewHost::beforePop() {
  if (m_listModel) {
    m_listModel->beforePop();
  } else if (m_gridModel) {
    m_gridModel->beforePop();
  }
}

QString QmlExtensionViewHost::viewType() const { return m_viewType; }

QObject *QmlExtensionViewHost::contentModel() const {
  if (m_listModel) return m_listModel;
  if (m_gridModel) return m_gridModel;
  return nullptr;
}

bool QmlExtensionViewHost::isExtLoading() const { return m_isLoading; }

QString QmlExtensionViewHost::detailMarkdown() const { return m_detailMarkdown; }

QVariantList QmlExtensionViewHost::detailMetadata() const { return m_detailMetadata; }

void QmlExtensionViewHost::renderDetail(const RootDetailModel &model) {
  bool wasLoading = m_isLoading;
  m_isLoading = model.isLoading;
  if (wasLoading != m_isLoading) { emit isLoadingChanged(); emit suppressEmptyViewChanged(); }
  setLoading(model.isLoading);

  if (model.navigationTitle) { setNavigationTitle(*model.navigationTitle); }

  m_detailMarkdown = model.markdown;
  m_detailMetadata =
      model.metadata ? qml::metadataToVariantList(*model.metadata) : QVariantList{};
  emit detailContentChanged();

  m_detailActions = model.actions;
  if (model.actions) {
    auto notify = [this](const QString &handler, const QJsonArray &args) {
      notifyExtension(handler, args);
    };
    setActions(ExtensionActionPanelBuilder::build(*model.actions, notify, &m_submenuCache));
  }
}

QObject *QmlExtensionViewHost::formModel() const { return m_formModel; }

QString QmlExtensionViewHost::linkAccessoryText() const { return m_linkAccessoryText; }

QString QmlExtensionViewHost::linkAccessoryHref() const { return m_linkAccessoryHref; }

void QmlExtensionViewHost::renderForm(const FormModel &model) {
  bool wasLoading = m_isLoading;
  m_isLoading = model.isLoading;
  if (wasLoading != m_isLoading) { emit isLoadingChanged(); emit suppressEmptyViewChanged(); }
  setLoading(model.isLoading);

  if (model.navigationTitle) { setNavigationTitle(*model.navigationTitle); }

  m_formModel->setFormData(model);

  // Link accessory
  QString newLinkText, newLinkHref;
  if (model.searchBarAccessory) {
    if (auto *link = std::get_if<FormModel::LinkAccessoryModel>(&*model.searchBarAccessory)) {
      newLinkText = link->text;
      newLinkHref = link->target;
    }
  }
  if (newLinkText != m_linkAccessoryText || newLinkHref != m_linkAccessoryHref) {
    bool hadAccessory = !m_linkAccessoryText.isEmpty();
    m_linkAccessoryText = newLinkText;
    m_linkAccessoryHref = newLinkHref;
    emit linkAccessoryChanged();
    if (hadAccessory != !m_linkAccessoryText.isEmpty()) {
      emit searchAccessoryUrlChanged();
    }
  }

  // Action panel
  m_formActions = model.actions;
  if (model.actions) {
    auto notify = [this](const QString &handler, const QJsonArray &args) {
      notifyExtension(handler, args);
    };
    auto submit = [this]() -> std::expected<QJsonObject, QString> { return m_formModel->submit(); };
    setActions(ExtensionActionPanelBuilder::build(*model.actions, notify, &m_submenuCache,
                                                  ActionPanelState::ShortcutPreset::Form, submit));
  }
}

void QmlExtensionViewHost::notifyExtension(const QString &handler, const QJsonArray &args) {
  m_controller->notify(handler, args);
}
