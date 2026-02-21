#include "qml-shortcut-form-view-host.hpp"
#include "builtin_icon.hpp"
#include "favicon/favicon-service.hpp"
#include "navigation-controller.hpp"
#include "qml-utils.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/shortcut/shortcut-service.hpp"
#include "services/shortcut/shortcut.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-pannel/action.hpp"
#include <QFutureWatcher>
#include <QUrl>

QmlShortcutFormViewHost::QmlShortcutFormViewHost() : QmlFormViewBase() {}

QmlShortcutFormViewHost::QmlShortcutFormViewHost(std::shared_ptr<Shortcut> shortcut, Mode mode)
    : QmlFormViewBase(), m_mode(mode), m_initialShortcut(std::move(shortcut)) {}

QUrl QmlShortcutFormViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/ShortcutFormView.qml"));
}

QVariantMap QmlShortcutFormViewHost::qmlProperties() const {
  return {{QStringLiteral("host"), QVariant::fromValue(const_cast<QmlShortcutFormViewHost *>(this))}};
}

void QmlShortcutFormViewHost::setPrefilledValues(const QString &link, const QString &name,
                                                  const QString &application, const QString &icon) {
  m_prefilledLink = link;
  m_prefilledName = name;
  m_prefilledApp = application;
  m_prefilledIcon = icon;
}

void QmlShortcutFormViewHost::initialize() {
  BaseView::initialize();

  m_service = context()->services->shortcuts();
  m_appSelectorModel = new QmlAppSelectorModel(this);

  buildIconItems();
  buildLinkCompletions();

  // Set up action panel with submit action
  auto panel = std::make_unique<FormActionPanelState>();
  auto section = panel->createSection();
  auto submitAction =
      new StaticAction(QStringLiteral("Submit"), ImageURL::builtin("enter-key"), [this]() { submit(); });
  section->addAction(submitAction);
  setActions(std::move(panel));

  // Select defaults
  m_appSelectorModel->selectById(QStringLiteral("default"));
  m_selectedApp = m_appSelectorModel->currentItem();

  // Default icon
  m_selectedIcon = m_defaultIconEntry;

  // Populate fields from initial data if editing/duplicating
  if (m_initialShortcut) {
    auto appDb = context()->services->appDb();

    if (m_mode == Mode::Duplicate) {
      m_name = QString("Copy of %1").arg(m_initialShortcut->name());
    } else {
      m_name = m_initialShortcut->name();
    }

    m_link = m_initialShortcut->url();

    // Set app
    auto appId = m_initialShortcut->app();
    if (appDb->findById(appId)) {
      m_appSelectorModel->selectById(appId);
      m_selectedApp = m_appSelectorModel->currentItem();
    }

    // Set icon - try to find it in the icon items
    auto iconStr = m_initialShortcut->icon();
    bool iconFound = false;
    for (const auto &sectionVar : m_iconItems) {
      auto sectionMap = sectionVar.toMap();
      auto items = sectionMap[QStringLiteral("items")].toList();
      for (const auto &itemVar : items) {
        auto item = itemVar.toMap();
        if (item[QStringLiteral("id")].toString() == iconStr) {
          m_selectedIcon = item;
          iconFound = true;
          break;
        }
      }
      if (iconFound) break;
    }

    if (!iconFound) {
      // Trigger link blur to detect the icon
      handleLinkBlurred();
    }

    emit formChanged();
  } else if (!m_prefilledLink.isEmpty() || !m_prefilledName.isEmpty()) {
    // Apply prefilled values (from external sources like browser extension)
    m_name = m_prefilledName;
    m_link = m_prefilledLink;

    if (!m_prefilledApp.isEmpty()) {
      auto appDb = context()->services->appDb();
      if (appDb->findById(m_prefilledApp)) {
        m_appSelectorModel->selectById(m_prefilledApp);
        m_selectedApp = m_appSelectorModel->currentItem();
      }
    }

    if (!m_prefilledIcon.isEmpty()) {
      for (const auto &sectionVar : m_iconItems) {
        auto sectionMap = sectionVar.toMap();
        auto items = sectionMap[QStringLiteral("items")].toList();
        for (const auto &itemVar : items) {
          auto item = itemVar.toMap();
          if (item[QStringLiteral("id")].toString() == m_prefilledIcon) {
            m_selectedIcon = item;
            break;
          }
        }
      }
    }

    if (!m_prefilledLink.isEmpty()) {
      handleLinkBlurred();
    }

    emit formChanged();
  }

  // Set navigation title based on mode
  switch (m_mode) {
  case Mode::Create:
    break; // uses default from command
  case Mode::Edit:
    setNavigationTitle(QString("Edit \"%1\"").arg(m_initialShortcut->name()));
    break;
  case Mode::Duplicate:
    setNavigationTitle(QString("Duplicate \"%1\"").arg(m_initialShortcut->name()));
    break;
  }
}

void QmlShortcutFormViewHost::buildIconItems() {
  QVariantList allIcons;

  // Default icon entry
  m_defaultIconEntry = QVariantMap{
      {QStringLiteral("id"), QStringLiteral("default")},
      {QStringLiteral("displayName"), QStringLiteral("Default")},
      {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin("link"))},
  };
  allIcons.append(m_defaultIconEntry);

  for (const auto &[icon, name] : BuiltinIconService::mapping()) {
    auto url = ImageURL::builtin(name);
    allIcons.append(QVariantMap{
        {QStringLiteral("id"), url.toString()},
        {QStringLiteral("displayName"), QString::fromUtf8(name)},
        {QStringLiteral("iconSource"), qml::imageSourceFor(url)},
    });
  }

  QVariantMap section;
  section[QStringLiteral("title")] = QString();
  section[QStringLiteral("items")] = allIcons;
  m_iconItems.append(section);
}

void QmlShortcutFormViewHost::buildLinkCompletions() {
  m_linkCompletions = QVariantList{
      QVariantMap{
          {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin("text-cursor"))},
          {QStringLiteral("title"), QStringLiteral("Selected Text")},
          {QStringLiteral("value"), QStringLiteral("selected")},
      },
      QVariantMap{
          {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin("copy-clipboard"))},
          {QStringLiteral("title"), QStringLiteral("Clipboard Text")},
          {QStringLiteral("value"), QStringLiteral("clipboard")},
      },
      QVariantMap{
          {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin("text-cursor"))},
          {QStringLiteral("title"), QStringLiteral("Argument")},
          {QStringLiteral("value"), QStringLiteral("argument")},
      },
      QVariantMap{
          {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin("fingerprint"))},
          {QStringLiteral("title"), QStringLiteral("UUID")},
          {QStringLiteral("value"), QStringLiteral("uuid")},
      },
  };
}

void QmlShortcutFormViewHost::updateDefaultIconInItems() {
  if (m_iconItems.isEmpty()) return;
  auto section = m_iconItems[0].toMap();
  auto items = section[QStringLiteral("items")].toList();
  if (!items.isEmpty()) {
    items[0] = m_defaultIconEntry;
    section[QStringLiteral("items")] = items;
    m_iconItems[0] = section;
    emit iconItemsChanged();
  }
}

void QmlShortcutFormViewHost::submit() {
  auto toast = context()->services->toastService();

  // Clear previous errors
  m_linkError.clear();
  m_appError.clear();
  m_iconError.clear();

  bool valid = true;

  if (m_link.isEmpty()) {
    m_linkError = QStringLiteral("Required");
    valid = false;
  }

  if (m_selectedApp.isEmpty()) {
    m_appError = QStringLiteral("Required");
    valid = false;
  }

  if (m_selectedIcon.isEmpty()) {
    m_iconError = QStringLiteral("Required");
    valid = false;
  }

  emit errorsChanged();

  if (!valid) {
    toast->failure("Validation failed");
    return;
  }

  auto appId = m_selectedApp[QStringLiteral("id")].toString();
  auto iconId = m_selectedIcon[QStringLiteral("id")].toString();

  // Resolve "default" app to actual app id
  if (appId == QStringLiteral("default")) {
    auto appDb = context()->services->appDb();
    if (auto browser = appDb->webBrowser()) {
      appId = browser->id();
    }
  }

  // Resolve "default" icon
  if (iconId == QStringLiteral("default")) {
    iconId = ImageURL::builtin("link").toString();
  }

  if (m_mode == Mode::Edit) {
    bool updated = m_service->updateShortcut(m_initialShortcut->id(), m_name, iconId, m_link, appId);
    if (!updated) {
      toast->failure("Failed to update shortcut");
      return;
    }
    toast->success("Shortcut updated");
  } else {
    bool created = m_service->createShortcut(m_name, iconId, m_link, appId);
    if (!created) {
      toast->failure("Failed to create shortcut");
      return;
    }
    toast->success("Shortcut created");
  }

  popSelf();
}

void QmlShortcutFormViewHost::handleLinkBlurred() {
  auto appDb = context()->services->appDb();
  QUrl url(m_link);

  if (auto app = appDb->findDefaultOpener(m_link)) {
    m_appSelectorModel->updateDefaultApp(app);
    m_selectedApp = m_appSelectorModel->currentItem();

    // Also update the default icon entry with the app icon
    m_defaultIconEntry[QStringLiteral("iconSource")] = qml::imageSourceFor(app->iconUrl());
    m_defaultIconEntry[QStringLiteral("displayName")] = QStringLiteral("Default");
    updateDefaultIconInItems();

    if (m_selectedIcon[QStringLiteral("id")].toString() == QStringLiteral("default")) {
      m_selectedIcon = m_defaultIconEntry;
    }
    emit formChanged();
  }

  if (url.scheme().startsWith("http")) {
    using Watcher = QFutureWatcher<FaviconService::FaviconResponse>;

    auto watcher = std::make_unique<Watcher>();
    auto ptr = watcher.get();

    watcher->setFuture(FaviconService::instance()->makeRequest(url.host()));
    connect(ptr, &Watcher::finished, this, [this, url, watcher = std::move(watcher)]() {
      auto icon = ImageURL::favicon(url.host()).withFallback(ImageURL::builtin("image"));
      m_defaultIconEntry[QStringLiteral("iconSource")] = qml::imageSourceFor(icon);
      m_defaultIconEntry[QStringLiteral("displayName")] = url.host();
      updateDefaultIconInItems();

      if (m_selectedIcon[QStringLiteral("id")].toString() == QStringLiteral("default")) {
        m_selectedIcon = m_defaultIconEntry;
        emit formChanged();
      }
    });
  }
}

void QmlShortcutFormViewHost::selectApp(const QVariantMap &item) {
  m_appSelectorModel->select(item);
  m_selectedApp = item;

  // Update default icon when app changes
  if (!m_link.isEmpty()) {
    auto iconSource = item[QStringLiteral("iconSource")].toString();
    m_defaultIconEntry[QStringLiteral("iconSource")] = iconSource;
    m_defaultIconEntry[QStringLiteral("displayName")] = QStringLiteral("Default");
    updateDefaultIconInItems();

    if (m_selectedIcon[QStringLiteral("id")].toString() == QStringLiteral("default")) {
      m_selectedIcon = m_defaultIconEntry;
    }
  }

  emit formChanged();
}

void QmlShortcutFormViewHost::selectIcon(const QVariantMap &item) {
  m_selectedIcon = item;
  emit formChanged();
}

