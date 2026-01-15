#include "store-detail-view.hpp"
#include "common.hpp"
#include "navigation-controller.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "ui/screenshot-list/screenshot-list.hpp"
#include "ui/thumbnail/thumbnail.hpp"
#include "utils.hpp"
#include "services/toast/toast-service.hpp"

Stack RaycastStoreDetailView::createHeader() {
  auto author = HStack()
                    .addIcon(ImageURL::http(m_ext.author.avatar).circle(), {16, 16})
                    .addText(m_ext.author.name)
                    .spacing(10);
  auto downloadCount = HStack()
                           .addIcon(ImageURL::builtin("arrow-down-circle"), {16, 16})
                           .addText(formatCount(m_ext.download_count))
                           .spacing(5);

  auto metadata = HStack()
                      .add(author)
                      .add(downloadCount)
                      .addIf(!m_ext.platforms.empty(),
                             [&]() {
                               auto platforms = HStack().spacing(5);

                               if (m_ext.platforms.contains("macOS")) {
                                 platforms.addIcon(ImageURL::builtin("apple"), {16, 16});
                               }
                               if (m_ext.platforms.contains("Windows")) {
                                 platforms.addIcon(ImageURL::builtin("windows11"), {16, 16});
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

  m_installedAccessory->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  m_installedAccessory->setAccessory({
      .text = "Installed",
      .color = SemanticColor::Green,
      .fillBackground = true,
      .icon = ImageURL::builtin("check-circle"),
  });

  m_installedAccessory->setMaximumHeight(30);

  return HStack().add(left).add(m_installedAccessory).justifyBetween();
}

QWidget *RaycastStoreDetailView::createPresentationSection() {
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

Stack RaycastStoreDetailView::createContributorList() {
  auto makeContributor = [&](const Raycast::User &user) {
    return HStack().addIcon(user.validUserIcon().circle(), {16, 16}).addText(user.name).spacing(10);
  };

  return VStack().map(m_ext.contributors, makeContributor).spacing(10);
}

Stack RaycastStoreDetailView::createMainWidget() {
  return VStack()
      .add(VStack()
               .addText("Description")
               .addParagraph(m_ext.description, SemanticColor::TextMuted)
               .spacing(10))
      .add(VStack()
               .addText("Commands", SemanticColor::TextMuted)
               .add(VStack()
                        .map(m_ext.commands,
                             [&](const auto &cmd) {
                               return VStack()
                                   .add(HStack()
                                            .addIcon(cmd.themedIcon(), {20, 20})
                                            .addText(cmd.title)
                                            .spacing(10))
                                   .addParagraph(cmd.description, SemanticColor::TextMuted)
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

QWidget *RaycastStoreDetailView::createSideMetadataSection() {
  auto readmeLink = VStack()
                        .addText("README", SemanticColor::TextMuted)
                        .add(new TextLinkWidget("Open README", QUrl(m_ext.readme_assets_path)))
                        .spacing(5);
  auto viewSource = VStack()
                        .addText("Source Code", SemanticColor::TextMuted)
                        .add(new TextLinkWidget("View Code", QUrl(m_ext.source_url)))
                        .spacing(5);

  auto lastUpdate = VStack()
                        .addText("Last update", SemanticColor::TextMuted)
                        .addText(getRelativeTimeString(m_ext.updatedAtDateTime()), SemanticColor::Foreground)
                        .spacing(5);

  return VStack()
      .add(readmeLink)
      .add(lastUpdate)
      .addIf(!m_ext.contributors.empty(),
             [&]() {
               return VStack()
                   .addText("Contributors", SemanticColor::TextMuted)
                   .add(createContributorList())
                   .spacing(5);
             })
      .add(viewSource)
      .addStretch()
      .spacing(15)
      .margins(10)
      .buildWidget();
}

QWidget *RaycastStoreDetailView::createContentSection() {
  return HStack()
      .add(createMainWidget().buildWidget(), 2)
      .add(createSideMetadataSection(), 1)
      .divided(1)
      .buildWidget();
}

void RaycastStoreDetailView::handleClickedScreenshot(const ImageURL &url) {
  auto dialog = new ImagePreviewDialogWidget(url);

  dialog->setAspectRatio(16 / 10.f);
  context()->navigation->setDialog(dialog);
}

void RaycastStoreDetailView::createActions() {
  auto panel = std::make_unique<ListActionPanelState>();
  auto registry = context()->services->extensionRegistry();
  bool isInstalled = registry->isInstalled(m_ext.id);
  auto main = panel->createSection();

  if (!isInstalled) {
    auto install = new StaticAction(
        "Install extension", m_ext.themedIcon(), [ext = m_ext](const ApplicationContext *ctx) {
          using Watcher = QFutureWatcher<Raycast::DownloadExtensionResult>;
          auto store = ctx->services->raycastStore();
          auto watcher = new Watcher;
          auto toast = ctx->services->toastService();
          auto registry = ctx->services->extensionRegistry();

          toast->dynamic("Downloading extension...");

          QObject::connect(watcher, &Watcher::finished, [ctx, registry, toast, ext, watcher]() {
            auto result = watcher->result();

            watcher->deleteLater();

            if (!result) {
              toast->failure("Failed to download extension");
              return;
            }

            registry->installFromZip(QString("store.raycast.%1").arg(ext.name), result->toStdString(),
                                     [toast](bool ok) {
                                       if (!ok) {
                                         toast->failure("Failed to extract extension archive");
                                         return;
                                       }
                                       toast->success("Extension installed");
                                     });
          });

          auto downloadResult = store->downloadExtension(ext.download_url);
          watcher->setFuture(downloadResult);
        });
    main->addAction(install);
  } else {
    auto uninstall = new UninstallExtensionAction(m_ext.id);
    main->addAction(uninstall);
  }

  setActions(std::move(panel));
}

void RaycastStoreDetailView::initialize() {
  auto registry = context()->services->extensionRegistry();
  bool isInstalled = registry->isInstalled(m_ext.id);

  if (isInstalled) {
    extensionInstalled();
  } else {
    extensionUninstalled();
  }

  createActions();
  QTimer::singleShot(0, this, [this]() { m_scrollArea->setFocus(); });

  connect(registry, &ExtensionRegistry::extensionAdded, this, [this](const QString &id) {
    if (id != m_ext.id) return;
    extensionInstalled();
    createActions();
  });

  connect(registry, &ExtensionRegistry::extensionUninstalled, this, [this](const QString &id) {
    if (id != m_ext.id) return;
    extensionUninstalled();
    createActions();
  });
}

bool RaycastStoreDetailView::supportsSearch() const { return false; }

QString RaycastStoreDetailView::initialNavigationTitle() const {
  return QString("%1 - %2").arg(BaseView::initialNavigationTitle()).arg(m_ext.title);
}

QWidget *RaycastStoreDetailView::createUI(const Raycast::Extension &ext) {
  return VStack()
      .add(createPresentationSection())
      .add(createContentSection())
      .addStretch()
      .divided(1)
      .buildWidget();
}

void RaycastStoreDetailView::setupUI(const Raycast::Extension &extension) {
  m_scrollArea->setWidget(createUI(extension));
  m_scrollArea->setFocusPolicy(Qt::StrongFocus);
  VStack().add(m_scrollArea).imbue(this);
}
