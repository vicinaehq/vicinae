#include "actions/extension/extension-actions.hpp"
#include "ui/views/base-view.hpp"
#include "extensions/raycast/store/store-detail-view.hpp"
#include "navigation-controller.hpp"
#include "ui/image/url.hpp"
#include "services/raycast/raycast-store.hpp"
#include "services/toast/toast-service.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "theme.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/list-accessory/list-accessory.hpp"
#include "ui/toast/toast.hpp"
#include "ui/views/list-view.hpp"
#include "utils.hpp"
#include <chrono>
#include <memory>
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qwidget.h>
#include "store-listing-view.hpp"

class RaycastStoreExtensionItemWidget : public SelectableOmniListWidget {
public:
  RaycastStoreExtensionItemWidget(QWidget *parent = nullptr) : SelectableOmniListWidget(parent) {
    auto left = HStack().spacing(15).add(m_icon).add(VStack().add(m_title).add(m_description).spacing(2));
    auto right = HStack().add(m_installed).add(m_downloadCount).add(m_author).spacing(10);
    auto layout = HStack().add(left).add(right).justifyBetween().margins(10).spacing(15);

    m_installed->setUrl(ImageURL::builtin("check-circle").setFill(SemanticColor::Green));
    m_installed->setFixedSize(20, 20);

    m_downloadCount->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_author->setFixedSize(20, 20);
    m_description->setColor(SemanticColor::TextMuted);
    m_icon->setFixedSize(30, 30);
    layout.imbue(this);
  }

  void setIcon(const ImageURL &url) { m_icon->setUrl(url); }
  void setTitle(const QString &title) { m_title->setText(title); }
  void setDescription(const QString &description) { m_description->setText(description); }
  void setAuthorUrl(const ImageURL &url) { m_author->setUrl(url); }
  void setDownloadCount(int count) {
    m_downloadCount->setAccessory(
        ListAccessory{.text = formatCount(count), .icon = ImageURL::builtin("arrow-down-circle")});
  }
  void setInstalled(bool value) { m_installed->setVisible(value); }

private:
  ImageWidget *m_icon = new ImageWidget();
  TypographyWidget *m_title = new TypographyWidget();
  TypographyWidget *m_description = new TypographyWidget();
  ImageWidget *m_installed = new ImageWidget;
  ImageWidget *m_author = new ImageWidget;
  ListAccessoryWidget *m_downloadCount = new ListAccessoryWidget;
};

class RaycastStoreExtensionItem : public OmniList::AbstractVirtualItem, public ListView::Actionnable {
  Raycast::Extension m_extension;
  bool m_installed = false;

public:
  bool hasUniformHeight() const override { return true; }

  QString generateId() const override { return m_extension.id; }

  bool recyclable() const override { return false; }

  void refresh(QWidget *widget) const override {
    imbue(static_cast<RaycastStoreExtensionItemWidget *>(widget));
  }

  void imbue(RaycastStoreExtensionItemWidget *item) const {
    item->setTitle(m_extension.title);
    item->setDescription(m_extension.description);
    item->setIcon(m_extension.themedIcon());
    item->setDownloadCount(m_extension.download_count);
    item->setInstalled(m_installed);

    if (m_extension.author.avatar.isEmpty()) {
      item->setAuthorUrl(ImageURL::builtin("person"));
    } else {
      item->setAuthorUrl(ImageURL::http(m_extension.author.avatar).setMask(OmniPainter::CircleMask));
    }
  }

  OmniListItemWidget *createWidget() const override {
    auto item = new RaycastStoreExtensionItemWidget;

    imbue(item);

    return item;
  }

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const override {
    auto panel = std::make_unique<ActionPanelState>();
    auto section = panel->createSection();
    auto danger = panel->createSection();
    auto icon = m_extension.themedIcon();
    auto showExtension = new StaticAction(
        "Show details", ImageURL::builtin("computer-chip"),
        [ext = m_extension, icon, ctx]() { ctx->navigation->pushView(new RaycastStoreDetailView(ext)); });
    auto uninstall = new UninstallExtensionAction(m_extension.id);

    showExtension->setShortcut(Keyboard::Shortcut::enter());
    uninstall->setShortcut(Keybind::RemoveAction);

    panel->setTitle(m_extension.name);
    section->addAction(showExtension);
    danger->addAction(uninstall);
    showExtension->setPrimary(true);

    return panel;
  }

  RaycastStoreExtensionItem(const Raycast::Extension &extension, bool installed)
      : m_extension(extension), m_installed(installed) {}
};

RaycastStoreListingView::RaycastStoreListingView() {
  m_debounce.setSingleShot(true);
  m_debounce.setInterval(std::chrono::milliseconds(200));

  connect(&m_debounce, &QTimer::timeout, this, &RaycastStoreListingView::handleDebounce);
  connect(&m_listResultWatcher, &QFutureWatcher<Raycast::ListResult>::finished, this,
          &RaycastStoreListingView::handleFinishedPage);
  connect(&m_queryResultWatcher, &QFutureWatcher<Raycast::ListResult>::finished, this,
          &RaycastStoreListingView::handleFinishedQuery);
}

void RaycastStoreListingView::initialize() {
  auto registry = context()->services->extensionRegistry();

  m_store = context()->services->raycastStore();
  setLoading(true);
  setSearchPlaceholderText("Browse Raycast extensions");

  auto result = m_store->fetchExtensions();

  m_listResultWatcher.setFuture(result);

  connect(registry, &ExtensionRegistry::extensionsChanged, this, &RaycastStoreListingView::refresh);
}

void RaycastStoreListingView::handleFinishedQuery() {
  if (searchText() != lastQueryText) return;

  auto result = m_queryResultWatcher.result();

  if (!result) {
    context()->services->toastService()->setToast("Failed to search extensions", ToastStyle::Danger);
    return;
  }

  setLoading(false);

  m_list->updateModel([&]() {
    auto &results = m_list->addSection("Results");

    for (const auto &extension : result->m_extensions) {
      bool installed = context()->services->extensionRegistry()->isInstalled(extension.id);
      results.addItem(std::make_unique<RaycastStoreExtensionItem>(extension, installed));
    }
  });
}

void RaycastStoreListingView::refresh() { setSearchText(searchText()); }

void RaycastStoreListingView::handleDebounce() {
  if (searchText().isEmpty()) return;

  setLoading(true);
  lastQueryText = searchText();
  auto result = m_store->search(lastQueryText);

  m_queryResultWatcher.setFuture(result);
}

void RaycastStoreListingView::textChanged(const QString &text) {
  if (text.isEmpty()) {
    setLoading(true);
    auto result = m_store->fetchExtensions();

    m_listResultWatcher.setFuture(result);
    return;
  }

  m_debounce.start();
}

void RaycastStoreListingView::handleFinishedPage() {
  if (!searchText().isEmpty()) return;

  auto result = m_listResultWatcher.result();

  if (!result) {
    context()->services->toastService()->setToast("Failed to fetch extensions", ToastStyle::Danger);
    return;
  }

  setLoading(false);

  m_list->updateModel([&]() {
    auto &results = m_list->addSection("Extensions");

    for (const auto &extension : result->m_extensions) {
      bool installed = context()->services->extensionRegistry()->isInstalled(extension.id);

      results.addItem(std::make_unique<RaycastStoreExtensionItem>(extension, installed));
    }
  });
}
