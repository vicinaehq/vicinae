#pragma once
#include "ui/views/base-view.hpp"
#include "common.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/text-link/text-link.hpp"
#include "../../../ui/image/url.hpp"
#include "services/raycast/raycast-store.hpp"
#include "settings/extension-settings.hpp"
#include "theme.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/list-accessory/list-accessory.hpp"
#include "ui/scroll-bar/scroll-bar.hpp"
#include "ui/thumbnail/thumbnail.hpp"
#include "ui/typography/typography.hpp"
#include "utils/layout.hpp"
#include "utils/utils.hpp"
#include "zip/unzip.hpp"
#include <absl/container/internal/raw_hash_set.h>
#include <qboxlayout.h>
#include "services/extension-registry/extension-registry.hpp"
#include "services/toast/toast-service.hpp"
#include <qevent.h>
#include <qfuturewatcher.h>
#include <qnamespace.h>
#include <qscrollarea.h>
#include <qsizepolicy.h>
#include <qtmetamacros.h>
#include <qurl.h>
#include <qwidget.h>

class ScreenshotList : public QScrollArea {
  Q_OBJECT

  QWidget *createWidget(const std::vector<QUrl> &urls) {
    auto makeThumbnail = [this](const QUrl &url) -> Thumbnail * {
      auto thumbnail = new Thumbnail;
      double aspectRatio = 16 / 10.f;

      connect(thumbnail, &Thumbnail::clicked, this, [this, url]() { emit clickedUrl(HttpOmniIconUrl(url)); });

      thumbnail->setClickable(true);
      thumbnail->setFixedHeight(150);
      thumbnail->setFixedWidth(150 * aspectRatio);
      thumbnail->setImage(HttpOmniIconUrl(url));

      return thumbnail;
    };

    return HStack().map(urls, makeThumbnail).addStretch().spacing(10).buildWidget();
  }

  void setupUI() {
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBar(new OmniScrollBar);
  }

public:
  void setUrls(const std::vector<QUrl> &urls) {
    setWidget(createWidget(urls));
    setWidgetResizable(true);
    setAutoFillBackground(true);
  }

  ScreenshotList() { setupUI(); }

signals:
  void clickedUrl(const ImageURL &url);
};

class RaycastStoreDetailView : public BaseView {
  VerticalScrollArea *m_scrollArea = new VerticalScrollArea(this);
  Raycast::Extension m_ext;
  ListAccessoryWidget *m_installedAccessory = new ListAccessoryWidget;

  bool supportsSearch() const override { return false; }

  Stack createContributorList() {
    auto makeContributor = [&](const Raycast::User &user) {
      return HStack().addIcon(user.validUserIcon().circle(), {16, 16}).addText(user.name).spacing(10);
    };

    return VStack().map(m_ext.contributors, makeContributor).spacing(10);
  }

  Stack createHeader() {
    auto author = HStack()
                      .addIcon(HttpOmniIconUrl(m_ext.author.avatar).circle(), {16, 16})
                      .addText(m_ext.author.name)
                      .spacing(10);
    auto downloadCount = HStack()
                             .addIcon(BuiltinOmniIconUrl("arrow-down-circle"), {16, 16})
                             .addText(formatCount(m_ext.download_count))
                             .spacing(5);

    auto metadata = HStack()
                        .add(author)
                        .add(downloadCount)
                        .addIf(!m_ext.platforms.isEmpty(),
                               [&]() {
                                 auto platforms = HStack().spacing(5);

                                 if (m_ext.platforms.contains("macOS")) {
                                   platforms.addIcon(BuiltinOmniIconUrl("apple"), {16, 16});
                                 }
                                 if (m_ext.platforms.contains("Windows")) {
                                   platforms.addIcon(BuiltinOmniIconUrl("windows11"), {16, 16});
                                 }

                                 return platforms;
                               })
                        .addStretch()
                        .divided(1)
                        .spacing(10);

    auto left = HStack()
                    .addIcon(m_ext.themedIcon(), {64, 64})
                    .add(VStack().addTitle(m_ext.title).add(metadata).margins(0, 4, 0, 4).justifyBetween())
                    .spacing(20);

    m_installedAccessory->setAccessory({
        .text = "Installed",
        .color = SemanticColor::Green,
        .fillBackground = true,
        .icon = BuiltinOmniIconUrl("check-circle"),
    });

    m_installedAccessory->setMaximumHeight(30);

    return HStack().add(left).add(m_installedAccessory).justifyBetween();
  }

  Stack createMainWidget() {
    return VStack()
        .add(VStack()
                 .addText("Description")
                 .addParagraph(m_ext.description, SemanticColor::TextSecondary)
                 .spacing(10))
        .add(VStack()
                 .addText("Commands", SemanticColor::TextSecondary)
                 .add(VStack()
                          .map(m_ext.commands,
                               [&](const auto &cmd) {
                                 return VStack()
                                     .add(HStack()
                                              .addIcon(cmd.themedIcon(), {20, 20})
                                              .addText(cmd.title)
                                              .spacing(10))
                                     .addParagraph(cmd.description, SemanticColor::TextSecondary)
                                     .spacing(10);
                               })
                          .divided(1)
                          .spacing(15)

                          )
                 .spacing(20))
        .addStretch()
        .divided(1)
        .margins(20)
        .spacing(20);
  }

  // section with header and preview images
  QWidget *createPresentationSection() {
    auto header = createHeader().buildWidget();

    header->setMaximumHeight(70);

    return VStack()
        .add(header)
        .addIf(m_ext.metadata_count > 0,
               [&]() {
                 auto list = new ScreenshotList();
                 list->setFixedHeight(160);
                 list->setUrls(m_ext.screenshots());

                 connect(list, &ScreenshotList::clickedUrl, this,
                         &RaycastStoreDetailView::handleClickedScreenshot);

                 return list;
               })
        .spacing(20)
        .divided(1)
        .margins(25)
        .buildWidget();
  }

  QWidget *createSideMetadataSection() {
    auto readmeLink = VStack()
                          .addText("README", SemanticColor::TextSecondary)
                          .add(new TextLinkWidget("Open README", QUrl(m_ext.readme_assets_path)))
                          .spacing(5);
    auto viewSource = VStack()
                          .addText("Source Code", SemanticColor::TextSecondary)
                          .add(new TextLinkWidget("View Code", QUrl(m_ext.source_url)))
                          .spacing(5);

    auto lastUpdate =
        VStack()
            .addText("Last update", SemanticColor::TextSecondary)
            .addText(getRelativeTimeString(m_ext.updatedAtDateTime()), SemanticColor::TextPrimary)
            .spacing(5);

    return VStack()
        .add(readmeLink)
        .add(lastUpdate)
        .addIf(!m_ext.contributors.isEmpty(),
               [&]() {
                 return VStack()
                     .addText("Contributors", SemanticColor::TextSecondary)
                     .add(createContributorList())
                     .spacing(5);
               })
        .add(viewSource)
        .addStretch()
        .spacing(15)
        .margins(10)
        .buildWidget();
  }

  QWidget *createContentSection() {
    return HStack()
        .add(createMainWidget().buildWidget(), 2)
        .add(createSideMetadataSection(), 1)
        .divided(1)
        .buildWidget();
  }

  void handleClickedScreenshot(const ImageURL &url) {
    auto dialog = new ImagePreviewDialogWidget(url);

    dialog->setAspectRatio(16 / 10.f);
    context()->navigation->setDialog(dialog);
  }

  QWidget *createUI(const Raycast::Extension &ext) {
    return VStack().add(createPresentationSection()).add(createContentSection()).divided(1).buildWidget();
  }

  void createActions() {
    auto panel = std::make_unique<ActionPanelState>();
    auto install = new StaticAction(
        "Install extension", m_ext.themedIcon(), [ext = m_ext](const ApplicationContext *ctx) {
          using Watcher = QFutureWatcher<Raycast::DownloadExtensionResult>;
          auto store = ctx->services->raycastStore();
          auto watcher = new Watcher;
          ctx->services->toastService()->setToast("Downloading extension...");

          QObject::connect(watcher, &Watcher::finished, [ctx, ext, watcher]() {
            auto result = watcher->result();

            watcher->deleteLater();

            if (!result) { ctx->services->toastService()->setToast("Failed to download extension"); }

            ctx->services->extensionRegistry()->installFromZip(ext.id,
                                                               std::string_view(result->toStdString()));
            ctx->services->toastService()->setToast("Extension downloaded");
          });

          auto downloadResult = store->downloadExtension(ext.download_url);

          watcher->setFuture(downloadResult);
        });

    auto main = panel->createSection();

    main->addAction(install);
    install->setPrimary(true);

    setActions(std::move(panel));
  }

  void initialize() override {
    bool isInstalled = context()->services->extensionRegistry()->isInstalled(m_ext.id);

    createActions();
    m_installedAccessory->setVisible(isInstalled);
  }

  void setupUI(const Raycast::Extension &extension) {
    auto layout = new QVBoxLayout;

    m_scrollArea->setWidget(createUI(extension));
    layout->addWidget(m_scrollArea);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
  }

public:
  RaycastStoreDetailView(const Raycast::Extension &extension) : m_ext(extension) {
    setupUI(extension);

    for (const auto &url : extension.screenshots()) {
      qDebug() << "asset url" << url;
    }
  }
};
