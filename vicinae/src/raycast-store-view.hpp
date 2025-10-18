#pragma once
#include "ui/views/base-view.hpp"
#include "navigation-controller.hpp"
#include "omni-icon.hpp"
#include "services/raycast/raycast-store.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include "ui/list-accessory-widget.hpp"
#include "ui/toast/toast.hpp"
#include <chrono>
#include <memory>
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qwidget.h>

class RaycastStoreExtensionItemWidget : public SelectableOmniListWidget {
  QHBoxLayout *m_layout = new QHBoxLayout(this);
  ImageWidget *m_icon = new ImageWidget();
  TypographyWidget *m_title = new TypographyWidget();
  TypographyWidget *m_description = new TypographyWidget();
  AccessoryListWidget *m_accessories = new AccessoryListWidget(this);
  QWidget *m_textWidget = new QWidget(this);
  QVBoxLayout *m_textLayout = new QVBoxLayout(m_textWidget);
  ImageWidget *m_author = new ImageWidget;
  ListAccessoryWidget *m_downloadCount = new ListAccessoryWidget;

public:
  void setIcon(const OmniIconUrl &url) { m_icon->setUrl(url); }
  void setTitle(const QString &title) { m_title->setText(title); }
  void setDescription(const QString &description) { m_description->setText(description); }
  void setAuthorUrl(const OmniIconUrl &url) { m_author->setUrl(url); }
  void setDownloadCount(int count) {
    m_downloadCount->setAccessory(
        ListAccessory{.text = formatCount(count), .icon = ImageURL::builtin("arrow-down-circle")});
  }

  RaycastStoreExtensionItemWidget(QWidget *parent = nullptr) : SelectableOmniListWidget(parent) {
    m_downloadCount->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_author->setFixedSize(20, 20);
    m_description->setColor(ColorTint::TextMuted);
    m_textLayout->addWidget(m_title);
    m_textLayout->setContentsMargins(0, 0, 0, 0);
    m_textLayout->setSpacing(2);
    m_textLayout->addWidget(m_description);
    m_textWidget->setLayout(m_textLayout);

    m_icon->setFixedSize(30, 30);

    m_layout->setSpacing(15);
    m_layout->addWidget(m_icon);
    m_layout->addWidget(m_textWidget);
    m_layout->addStretch();
    m_layout->addWidget(m_downloadCount);
    m_layout->addWidget(m_author);
    m_layout->setContentsMargins(10, 10, 10, 10);
    m_layout->setAlignment(Qt::AlignVCenter);

    setLayout(m_layout);
  }
};

class RaycastExtensionDetailsView : public BaseView {};

class RaycastStoreExtensionItem : public OmniList::AbstractVirtualItem, public ListView::Actionnable {
  Raycast::Extension m_extension;

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
    auto icon = m_extension.themedIcon();
    auto showExtension = new StaticAction("Show details", ImageURL::builtin("computer-chip"),
                                          [ext = m_extension, icon, ctx]() {
                                            ctx->navigation->pushView(new RaycastExtensionDetailsView());
                                            ctx->navigation->setNavigationTitle(ext.name);
                                            ctx->navigation->setNavigationIcon(icon);
                                          });

    panel->setTitle(m_extension.name);
    section->addAction(showExtension);
    showExtension->setPrimary(true);

    return panel;
  }

  RaycastStoreExtensionItem(const Raycast::Extension &extension) : m_extension(extension) {}
};

class RaycastStoreListingView : public ListView {
  RaycastStoreService *m_store = nullptr;
  QFutureWatcher<Raycast::ListResult> m_listResultWatcher;
  QFutureWatcher<Raycast::ListResult> m_queryResultWatcher;
  QString lastQueryText;
  QTimer m_debounce;

  void handleDebounce() {
    if (searchText().isEmpty()) return;

    setLoading(true);
    lastQueryText = searchText();
    m_queryResultWatcher.setFuture(m_store->search(lastQueryText));
  }

  void textChanged(const QString &text) override {
    qDebug() << "Store search" << text;
    if (text.isEmpty()) {
      setLoading(true);
      m_listResultWatcher.setFuture(m_store->fetchExtensions());
      return;
    }

    m_debounce.start();
  }

  void handleFinishedQuery() {
    if (searchText() != lastQueryText) return;

    auto result = m_queryResultWatcher.result();

    if (!result) {
      context()->services->toastService()->setToast("Failed to search extensions", ToastPriority::Danger);
      return;
    }

    setLoading(false);

    m_list->updateModel([&]() {
      auto &results = m_list->addSection("Results");

      for (const auto &extension : result->m_extensions) {
        results.addItem(std::make_unique<RaycastStoreExtensionItem>(extension));
      }
    });
  }

  void handleFinishedPage() {
    if (!searchText().isEmpty()) return;

    auto result = m_listResultWatcher.result();

    if (!result) {
      context()->services->toastService()->setToast("Failed to fetch extensions", ToastPriority::Danger);
      return;
    }

    setLoading(false);

    m_list->updateModel([&]() {
      auto &results = m_list->addSection("Extensions");

      for (const auto &extension : result->m_extensions) {
        results.addItem(std::make_unique<RaycastStoreExtensionItem>(extension));
      }
    });
  }

public:
  void initialize() override {
    m_store = context()->services->raycastStore();
    setLoading(true);

    m_listResultWatcher.setFuture(m_store->fetchExtensions());
  }

  RaycastStoreListingView() {
    m_debounce.setSingleShot(true);
    m_debounce.setInterval(std::chrono::milliseconds(200));

    connect(&m_debounce, &QTimer::timeout, this, &RaycastStoreListingView::handleDebounce);
    connect(&m_listResultWatcher, &QFutureWatcher<Raycast::ListResult>::finished, this,
            &RaycastStoreListingView::handleFinishedPage);
    connect(&m_queryResultWatcher, &QFutureWatcher<Raycast::ListResult>::finished, this,
            &RaycastStoreListingView::handleFinishedQuery);
  }
};
