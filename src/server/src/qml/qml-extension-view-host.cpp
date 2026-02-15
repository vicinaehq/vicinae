#include "qml-extension-view-host.hpp"
#include "navigation-controller.hpp"
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

QVariantMap QmlExtensionViewHost::qmlProperties() const {
  return {{QStringLiteral("host"), QVariant::fromValue(const_cast<QmlExtensionViewHost *>(this))}};
}

void QmlExtensionViewHost::loadInitialData() {
  // Search text will be forwarded when first render arrives
}

void QmlExtensionViewHost::onReactivated() {
  if (m_listModel) {
    m_listModel->refreshActionPanel();
  } else if (m_gridModel) {
    m_gridModel->refreshActionPanel();
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
  } else {
    // Form or detail — we can't handle these, request fallback
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
    renderGrid(*gridModel);

    emit viewTypeChanged();
    emit contentModelChanged();
  } else {
    // Form or detail — request fallback to widget
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
  if (wasLoading != m_isLoading) { emit isLoadingChanged(); }
  setLoading(model.isLoading);

  if (model.dirty) { m_listModel->setExtensionData(model); }
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
  if (wasLoading != m_isLoading) { emit isLoadingChanged(); }
  setLoading(model.isLoading);

  if (model.dirty) { m_gridModel->setExtensionData(model); }
}

void QmlExtensionViewHost::textChanged(const QString &text) {
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

void QmlExtensionViewHost::notifyExtension(const QString &handler, const QJsonArray &args) {
  m_controller->notify(handler, args);
}
