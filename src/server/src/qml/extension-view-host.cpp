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
  if (auto *list = activeModel<ExtensionListModel>()) {
    list->refreshActionPanel();
  } else if (auto *grid = activeModel<ExtensionGridModel>()) {
    grid->refreshActionPanel();
  } else if (auto *detail = std::get_if<DetailState>(&m_model)) {
    if (detail->actions) {
      auto notify = [this](const QString &handler, const QJsonArray &args) {
        notifyExtension(handler, args);
      };
      setActions(ExtensionActionPanelBuilder::build(*detail->actions, notify, &m_submenuCache));
    }
  } else if (activeModel<ExtensionFormModel>() && m_formActions) {
    auto *form = activeModel<ExtensionFormModel>();
    auto notify = [this](const QString &handler, const QJsonArray &args) {
      notifyExtension(handler, args);
    };
    auto submit = [this, form]() -> std::expected<QJsonObject, QString> { return form->submit(); };
    setActions(ExtensionActionPanelBuilder::build(*m_formActions, notify, &m_submenuCache,
                                                  ActionPanelState::ShortcutPreset::Form, submit));
  }
}

void ExtensionViewHost::render(const RenderModel &model) {
  auto needsSwitch = [&]() -> bool {
    if (m_firstRender) return true;
    if (std::holds_alternative<ListModel>(model)) return !activeModel<ExtensionListModel>();
    if (std::holds_alternative<GridModel>(model)) return !activeModel<ExtensionGridModel>();
    if (std::holds_alternative<RootDetailModel>(model)) return !std::holds_alternative<DetailState>(m_model);
    if (std::holds_alternative<FormModel>(model)) return !activeModel<ExtensionFormModel>();
    return false;
  };

  if (needsSwitch()) {
    m_firstRender = false;
    switchViewType(model);
  }

  if (auto *listModel = std::get_if<ListModel>(&model)) {
    renderList(*listModel);
  } else if (auto *gridModel = std::get_if<GridModel>(&model)) {
    renderGrid(*gridModel);
  } else if (auto *detailModel = std::get_if<RootDetailModel>(&model)) {
    renderDetail(*detailModel);
  } else if (auto *formModel = std::get_if<FormModel>(&model)) {
    renderForm(*formModel);
  } else {
    qWarning() << "Extension sent unrecognized model type";
  }
}

void ExtensionViewHost::switchViewType(const RenderModel &model) {
  // Delete any QObject model owned by the previous variant
  std::visit([](auto &v) {
    using T = std::decay_t<decltype(v)>;
    if constexpr (std::is_pointer_v<T>) delete v;
  }, m_model);

  auto notify = [this](const QString &handler, const QJsonArray &args) {
    notifyExtension(handler, args);
  };

  if (std::holds_alternative<ListModel>(model)) {
    auto *m = new ExtensionListModel(notify, this);
    m->setScope(ViewScope(context(), this));
    m->initialize();
    m_model = m;
    setSearchInteractive(true);
  } else if (std::holds_alternative<GridModel>(model)) {
    auto *m = new ExtensionGridModel(notify, this);
    m->setScope(ViewScope(context(), this));
    m->initialize();
    m_model = m;
    setSearchInteractive(true);
  } else if (std::holds_alternative<RootDetailModel>(model)) {
    m_model = DetailState{};
    setSearchInteractive(false);
  } else if (std::holds_alternative<FormModel>(model)) {
    m_model = new ExtensionFormModel(notify, this);
    setSearchInteractive(false);
  }

  emit viewTypeChanged();
}

void ExtensionViewHost::renderList(const ListModel &model) {
  auto *list = activeModel<ExtensionListModel>();
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
    list->setExtensionData(model, m_shouldResetSelection);
    m_selectFirstOnReset = true;
    emit selectFirstOnResetChanged();
    m_shouldResetSelection = false;
  }
}

void ExtensionViewHost::renderGrid(const GridModel &model) {
  auto *grid = activeModel<ExtensionGridModel>();
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
    grid->setExtensionData(model, m_shouldResetSelection);
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

  if (auto *list = activeModel<ExtensionListModel>()) {
    list->setFilter(text);
  } else if (auto *grid = activeModel<ExtensionGridModel>()) {
    grid->setFilter(text);
  }

  if (m_onSearchTextChange) {
    m_shouldResetSelection = !m_filtering;
    notifyExtension(m_onSearchTextChange->c_str(), {text});
  }
}

bool ExtensionViewHost::inputFilter(QKeyEvent *event) {
  return false;
}

void ExtensionViewHost::beforePop() {
  if (auto *list = activeModel<ExtensionListModel>()) {
    list->beforePop();
  } else if (auto *grid = activeModel<ExtensionGridModel>()) {
    grid->beforePop();
  }
}

QString ExtensionViewHost::viewType() const {
  if (activeModel<ExtensionListModel>()) return QStringLiteral("list");
  if (activeModel<ExtensionGridModel>()) return QStringLiteral("grid");
  if (activeModel<ExtensionFormModel>()) return QStringLiteral("form");
  if (std::holds_alternative<DetailState>(m_model)) return QStringLiteral("detail");
  return QStringLiteral("loading");
}

QObject *ExtensionViewHost::contentModel() const {
  if (auto *list = activeModel<ExtensionListModel>()) return list;
  if (auto *grid = activeModel<ExtensionGridModel>()) return grid;
  if (auto *form = activeModel<ExtensionFormModel>()) return form;
  return nullptr;
}

bool ExtensionViewHost::isExtLoading() const { return m_isLoading; }

QString ExtensionViewHost::detailMarkdown() const {
  auto *detail = std::get_if<DetailState>(&m_model);
  return detail ? detail->markdown : QString();
}

QVariantList ExtensionViewHost::detailMetadata() const {
  auto *detail = std::get_if<DetailState>(&m_model);
  return detail ? detail->metadata : QVariantList{};
}

void ExtensionViewHost::renderDetail(const RootDetailModel &model) {
  auto *detail = std::get_if<DetailState>(&m_model);

  bool wasLoading = m_isLoading;
  m_isLoading = model.isLoading;
  if (wasLoading != m_isLoading) { emit isLoadingChanged(); emit suppressEmptyViewChanged(); }
  setLoading(model.isLoading);

  if (model.navigationTitle) { setNavigationTitle(*model.navigationTitle); }

  detail->markdown = model.markdown;
  detail->metadata = model.metadata ? qml::metadataToVariantList(*model.metadata) : QVariantList{};
  emit detailContentChanged();

  detail->actions = model.actions;
  if (model.actions) {
    auto notify = [this](const QString &handler, const QJsonArray &args) {
      notifyExtension(handler, args);
    };
    setActions(ExtensionActionPanelBuilder::build(*model.actions, notify, &m_submenuCache));
  }
}

QString ExtensionViewHost::linkAccessoryText() const { return m_linkAccessoryText; }

QString ExtensionViewHost::linkAccessoryHref() const { return m_linkAccessoryHref; }

void ExtensionViewHost::renderForm(const FormModel &model) {
  auto *form = activeModel<ExtensionFormModel>();

  bool wasLoading = m_isLoading;
  m_isLoading = model.isLoading;
  if (wasLoading != m_isLoading) { emit isLoadingChanged(); emit suppressEmptyViewChanged(); }
  setLoading(model.isLoading);

  if (model.navigationTitle) { setNavigationTitle(*model.navigationTitle); }

  form->setFormData(model);

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
    auto submit = [this, form]() -> std::expected<QJsonObject, QString> { return form->submit(); };
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
