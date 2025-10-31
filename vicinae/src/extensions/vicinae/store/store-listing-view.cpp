#include "store-listing-view.hpp"
#include "actions/extension/extension-actions.hpp"
#include "extensions/vicinae/store/store-detail-view.hpp"
#include "navigation-controller.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "services/extension-store/vicinae-store.hpp"
#include "services/toast/toast-service.hpp"
#include "theme.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/image/url.hpp"
#include "ui/list-accessory/list-accessory.hpp"
#include "ui/toast/toast.hpp"
#include "ui/views/base-view.hpp"
#include "ui/views/list-view.hpp"
#include "utils.hpp"
#include <chrono>
#include <memory>
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qwidget.h>

class VicinaeStoreExtensionItemWidget : public SelectableOmniListWidget {
public:
  VicinaeStoreExtensionItemWidget(QWidget *parent = nullptr) : SelectableOmniListWidget(parent) {
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

class VicinaeStoreExtensionItem : public OmniList::AbstractVirtualItem, public ListView::Actionnable {
  VicinaeStore::Extension m_extension;
  bool m_installed = false;

public:
  bool hasUniformHeight() const override { return true; }

  QString generateId() const override { return m_extension.id; }

  bool recyclable() const override { return false; }

  void refresh(QWidget *widget) const override {
    imbue(static_cast<VicinaeStoreExtensionItemWidget *>(widget));
  }

  void imbue(VicinaeStoreExtensionItemWidget *item) const {
    item->setTitle(m_extension.title);
    item->setDescription(m_extension.description);
    item->setIcon(ImageURL::http(m_extension.themedIcon()));
    item->setDownloadCount(m_extension.downloadCount);
    item->setInstalled(m_installed);

    if (m_extension.author.avatarUrl.isEmpty()) {
      item->setAuthorUrl(ImageURL::builtin("person"));
    } else {
      item->setAuthorUrl(ImageURL::http(m_extension.author.avatarUrl).setMask(OmniPainter::CircleMask));
    }
  }

  OmniListItemWidget *createWidget() const override {
    auto item = new VicinaeStoreExtensionItemWidget;

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
        [ext = m_extension, icon, ctx]() { ctx->navigation->pushView(new VicinaeStoreDetailView(ext)); });
    auto uninstall = new UninstallExtensionAction(m_extension.id);

    showExtension->setShortcut(Keyboard::Shortcut::enter());
    uninstall->setShortcut(Keybind::RemoveAction);

    panel->setTitle(m_extension.name);
    section->addAction(showExtension);
    danger->addAction(uninstall);
    showExtension->setPrimary(true);

    return panel;
  }

  VicinaeStoreExtensionItem(const VicinaeStore::Extension &extension, bool installed)
      : m_extension(extension), m_installed(installed) {}
};

VicinaeStoreListingView::VicinaeStoreListingView() {
  m_debounce.setSingleShot(true);
  m_debounce.setInterval(std::chrono::milliseconds(200));

  connect(&m_debounce, &QTimer::timeout, this, &VicinaeStoreListingView::handleDebounce);
  connect(&m_listResultWatcher, &QFutureWatcher<VicinaeStore::ListResult>::finished, this,
          &VicinaeStoreListingView::handleFinishedPage);
  connect(&m_queryResultWatcher, &QFutureWatcher<VicinaeStore::ListResult>::finished, this,
          &VicinaeStoreListingView::handleFinishedQuery);
}

void VicinaeStoreListingView::initialize() {
  auto registry = context()->services->extensionRegistry();

  m_store = context()->services->vicinaeStore();
  setLoading(true);
  setSearchPlaceholderText("Browse Vicinae extensions");

  auto result = m_store->fetchExtensions();

  m_listResultWatcher.setFuture(result);

  connect(registry, &ExtensionRegistry::extensionsChanged, this, &VicinaeStoreListingView::refresh);
}

void VicinaeStoreListingView::handleFinishedQuery() {
  if (searchText() != lastQueryText) return;

  auto result = m_queryResultWatcher.result();

  if (!result) {
    context()->services->toastService()->setToast("Failed to search extensions", ToastStyle::Danger);
    return;
  }

  setLoading(false);

  m_list->updateModel([&]() {
    auto &results = m_list->addSection("Results");

    for (const auto &extension : result->extensions) {
      bool installed = context()->services->extensionRegistry()->isInstalled(extension.id);

      results.addItem(std::make_unique<VicinaeStoreExtensionItem>(extension, installed));
    }
  });
}

void VicinaeStoreListingView::refresh() { setSearchText(searchText()); }

void VicinaeStoreListingView::handleDebounce() {
  if (searchText().isEmpty()) return;

  setLoading(true);
  lastQueryText = searchText();
  auto result = m_store->search(lastQueryText);

  m_queryResultWatcher.setFuture(result);
}

void VicinaeStoreListingView::textChanged(const QString &text) {
  if (text.isEmpty()) {
    setLoading(true);
    auto result = m_store->fetchExtensions();

    m_listResultWatcher.setFuture(result);
    return;
  }

  m_debounce.start();
}

void VicinaeStoreListingView::handleFinishedPage() {
  if (!searchText().isEmpty()) return;

  auto result = m_listResultWatcher.result();

  if (!result) {
    context()->services->toastService()->setToast("Failed to fetch extensions", ToastStyle::Danger);
    return;
  }

  setLoading(false);

  m_list->updateModel([&]() {
    auto &results = m_list->addSection("Extensions");

    for (const auto &extension : result->extensions) {
      bool installed = context()->services->extensionRegistry()->isInstalled(extension.id);

      results.addItem(std::make_unique<VicinaeStoreExtensionItem>(extension, installed));
    }
  });
}
