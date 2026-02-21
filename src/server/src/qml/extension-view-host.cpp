#include "extension-view-host.hpp"
#include "navigation-controller.hpp"
#include "view-utils.hpp"
#include <chrono>

static const std::chrono::milliseconds THROTTLE_DEBOUNCE_DURATION(300);

ExtensionViewHost::ExtensionViewHost(ExtensionCommandController *controller, QObject *parent)
    : ViewHostBase(), m_controller(controller), m_searchDebounce(new QTimer(this)) {
  m_searchDebounce->setSingleShot(true);
  connect(m_searchDebounce, &QTimer::timeout, this, &ExtensionViewHost::handleDebouncedSearch);
}

QUrl ExtensionViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/ExtensionView.qml"));
}

QUrl ExtensionViewHost::qmlSearchAccessoryUrl() const {
  if (!m_dropdownItems.isEmpty())
    return QUrl(QStringLiteral("qrc:/Vicinae/ExtensionDropdownAccessory.qml"));
  if (!m_linkAccessoryText.isEmpty())
    return QUrl(QStringLiteral("qrc:/Vicinae/FormLinkAccessory.qml"));
  return {};
}

QVariantMap ExtensionViewHost::qmlProperties() const {
  return {{QStringLiteral("host"), QVariant::fromValue(const_cast<ExtensionViewHost *>(this))}};
}

void ExtensionViewHost::loadInitialData() {
  setLoading(true);
  setSearchInteractive(false);
}

void ExtensionViewHost::onReactivated() {
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

void ExtensionViewHost::render(const RenderModel &model) {
  if (m_firstRender) {
    m_firstRender = false;
    handleFirstRender(model);
    return;
  }

  if (auto *listModel = std::get_if<ListModel>(&model)) {
    if (m_listModel) {
      renderList(*listModel);
    } else {
      qWarning() << "Extension sent list model but view was initialized with a different type";
    }
  } else if (auto *gridModel = std::get_if<GridModel>(&model)) {
    if (m_gridModel) {
      renderGrid(*gridModel);
    } else {
      qWarning() << "Extension sent grid model but view was initialized with a different type";
    }
  } else if (auto *detailModel = std::get_if<RootDetailModel>(&model)) {
    if (m_viewType == "detail") {
      renderDetail(*detailModel);
    } else {
      qWarning() << "Extension sent detail model but view was initialized with a different type";
    }
  } else if (auto *formModel = std::get_if<FormModel>(&model)) {
    if (m_formModel) {
      renderForm(*formModel);
    } else {
      qWarning() << "Extension sent form model but view was initialized with a different type";
    }
  } else {
    qWarning() << "Extension sent unrecognized model type";
  }
}

void ExtensionViewHost::handleFirstRender(const RenderModel &model) {
  if (auto *listModel = std::get_if<ListModel>(&model)) {
    auto notify = [this](const QString &handler, const QJsonArray &args) {
      notifyExtension(handler, args);
    };

    m_listModel = new ExtensionListModel(notify, this);
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

    m_gridModel = new ExtensionGridModel(notify, this);
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
    m_formModel = new ExtensionFormModel(notify, this);
    m_viewType = "form";
    setSearchInteractive(false);
    renderForm(*formModel);

    emit viewTypeChanged();
    emit formModelChanged();
  } else {
    qWarning() << "Extension sent unrecognized model type on first render";
  }
}

void ExtensionViewHost::renderList(const ListModel &model) {
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

  if (model.searchBarAccessory) {
    auto *dropdown = std::get_if<DropdownModel>(&*model.searchBarAccessory);
    updateDropdown(dropdown);
  } else {
    updateDropdown(nullptr);
  }

  if (model.dirty) {
    m_selectFirstOnReset = m_shouldResetSelection;
    emit selectFirstOnResetChanged();
    m_listModel->setExtensionData(model, m_shouldResetSelection);
    m_selectFirstOnReset = true;
    emit selectFirstOnResetChanged();
    m_shouldResetSelection = false;
  }
}

void ExtensionViewHost::renderGrid(const GridModel &model) {
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

  if (model.searchBarAccessory) {
    auto *dropdown = std::get_if<DropdownModel>(&*model.searchBarAccessory);
    updateDropdown(dropdown);
  } else {
    updateDropdown(nullptr);
  }

  if (model.dirty) {
    m_gridModel->setExtensionData(model, m_shouldResetSelection);
    m_shouldResetSelection = false;
  }
}

void ExtensionViewHost::textChanged(const QString &text) {
  bool had = m_hasSearchText;
  m_hasSearchText = !text.isEmpty();
  if (had != m_hasSearchText) emit suppressEmptyViewChanged();

  if (m_throttle) {
    m_searchDebounce->start();
  } else {
    handleDebouncedSearch();
  }
}

void ExtensionViewHost::handleDebouncedSearch() {
  auto text = searchText();

  if (m_listModel) {
    m_listModel->setFilter(text);
  } else if (m_gridModel) {
    m_gridModel->setFilter(text);
  }

  if (m_onSearchTextChange) {
    m_shouldResetSelection = !m_filtering;
    notifyExtension(m_onSearchTextChange->c_str(), {text});
  }
}

bool ExtensionViewHost::inputFilter(QKeyEvent *event) {
  // Grid views need arrow key handling — the QML side handles this via
  // GenericGridView's focus handling, so we don't need to intercept here.
  // List views are handled by GenericListView's focus handling.
  return false;
}

void ExtensionViewHost::beforePop() {
  if (m_listModel) {
    m_listModel->beforePop();
  } else if (m_gridModel) {
    m_gridModel->beforePop();
  }
}

QString ExtensionViewHost::viewType() const { return m_viewType; }

QObject *ExtensionViewHost::contentModel() const {
  if (m_listModel) return m_listModel;
  if (m_gridModel) return m_gridModel;
  return nullptr;
}

bool ExtensionViewHost::isExtLoading() const { return m_isLoading; }

QString ExtensionViewHost::detailMarkdown() const { return m_detailMarkdown; }

QVariantList ExtensionViewHost::detailMetadata() const { return m_detailMetadata; }

void ExtensionViewHost::renderDetail(const RootDetailModel &model) {
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

QObject *ExtensionViewHost::formModel() const { return m_formModel; }

QString ExtensionViewHost::linkAccessoryText() const { return m_linkAccessoryText; }

QString ExtensionViewHost::linkAccessoryHref() const { return m_linkAccessoryHref; }

void ExtensionViewHost::renderForm(const FormModel &model) {
  bool wasLoading = m_isLoading;
  m_isLoading = model.isLoading;
  if (wasLoading != m_isLoading) { emit isLoadingChanged(); emit suppressEmptyViewChanged(); }
  setLoading(model.isLoading);

  if (model.navigationTitle) { setNavigationTitle(*model.navigationTitle); }

  m_formModel->setFormData(model);

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

void ExtensionViewHost::updateDropdown(const DropdownModel *dropdown) {
  bool hadDropdown = !m_dropdownItems.isEmpty();

  if (!dropdown) {
    if (hadDropdown) {
      m_dropdownItems.clear();
      m_dropdownCurrentItem.clear();
      m_dropdownPlaceholder.clear();
      m_dropdownOnChange.reset();
      emit dropdownChanged();
      emit searchAccessoryUrlChanged();
    }
    return;
  }

  m_dropdownOnChange = dropdown->onChange;
  m_dropdownPlaceholder = dropdown->placeholder.value_or(QString());

  if (dropdown->dirty) {
    m_dropdownItems = qml::convertDropdownChildren(dropdown->children);
  }

  // Resolve current value: explicit value > stored value > defaultValue > first item
  QString resolvedValue;
  if (dropdown->value) {
    resolvedValue = *dropdown->value;
  } else if (!m_dropdownValue.isEmpty()) {
    resolvedValue = m_dropdownValue;
  } else if (dropdown->defaultValue) {
    resolvedValue = *dropdown->defaultValue;
  } else {
    auto first = qml::firstDropdownItemValue(dropdown->children);
    if (first) resolvedValue = *first;
  }

  m_dropdownValue = resolvedValue;

  // Find the matching item to set as currentItem
  QVariant newCurrentItem;
  for (const auto &section : m_dropdownItems) {
    auto sectionMap = section.toMap();
    auto items = sectionMap["items"].toList();
    for (const auto &item : items) {
      auto itemMap = item.toMap();
      if (itemMap["id"].toString() == resolvedValue) {
        newCurrentItem = item;
        break;
      }
    }
    if (newCurrentItem.isValid()) break;
  }
  m_dropdownCurrentItem = newCurrentItem;

  emit dropdownChanged();
  if (hadDropdown != !m_dropdownItems.isEmpty()) {
    emit searchAccessoryUrlChanged();
  }
}

void ExtensionViewHost::setDropdownValue(const QString &value) {
  m_dropdownValue = value;

  // Find the matching item
  for (const auto &section : m_dropdownItems) {
    auto sectionMap = section.toMap();
    auto items = sectionMap["items"].toList();
    for (const auto &item : items) {
      auto itemMap = item.toMap();
      if (itemMap["id"].toString() == value) {
        m_dropdownCurrentItem = item;
        emit dropdownChanged();
        if (m_dropdownOnChange) {
          notifyExtension(*m_dropdownOnChange, {value});
        }
        return;
      }
    }
  }
}

void ExtensionViewHost::notifyExtension(const QString &handler, const QJsonArray &args) {
  m_controller->notify(handler, args);
}
