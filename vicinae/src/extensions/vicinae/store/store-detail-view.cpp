#include "store-detail-view.hpp"
#include "common.hpp"
#include "navigation-controller.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "services/extension-store/vicinae-store.hpp"
#include "services/toast/toast-service.hpp"
#include "utils.hpp"
#include <qnamespace.h>

Stack VicinaeStoreDetailView::createHeader() {
  auto author = HStack()
                    .addIcon(ImageURL::http(m_ext.author.avatarUrl).circle(), {16, 16})
                    .addText(m_ext.author.name)
                    .spacing(10);
  auto downloadCount = HStack()
                           .addIcon(ImageURL::builtin("arrow-down-circle"), {16, 16})
                           .addText(formatCount(m_ext.downloadCount))
                           .spacing(5);

  auto metadata = HStack()
                      .add(author)
                      .add(downloadCount)
                      .addIf(!m_ext.platforms.empty(),
                             [&]() {
                               auto platforms = HStack().spacing(5);

                               if (std::ranges::contains(m_ext.platforms, "linux")) {
                                 platforms.addIcon(ImageURL::builtin("linux"), {16, 16});
                               }
                               if (std::ranges::contains(m_ext.platforms, "macOS")) {
                                 platforms.addIcon(ImageURL::builtin("apple"), {16, 16});
                               }
                               if (std::ranges::contains(m_ext.platforms, "windows")) {
                                 platforms.addIcon(ImageURL::builtin("windows11"), {16, 16});
                               }

                               return platforms;
                             })
                      .addStretch()
                      .divided(1)
                      .spacing(10);

  auto left = HStack()
                  .addIcon(ImageURL::http(m_ext.themedIcon()), {64, 64})
                  .add(VStack().addTitle(m_ext.title).add(metadata).margins(0, 4, 0, 4).justifyBetween())
                  .spacing(20);

  m_installedAccessory->setAccessory({
      .text = "Installed",
      .color = SemanticColor::Green,
      .fillBackground = true,
      .icon = ImageURL::builtin("check-circle"),
  });

  m_installedAccessory->setMaximumHeight(30);

  return HStack().add(left).add(m_installedAccessory).justifyBetween();
}

QWidget *VicinaeStoreDetailView::createPresentationSection() {
  auto header = createHeader().buildWidget();

  header->setMaximumHeight(70);

  return VStack().add(header).spacing(20).divided(1).margins(25).buildWidget();
}

Stack VicinaeStoreDetailView::createMainWidget() {
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
                                            .addIcon(
                                                ImageURL::http(cmd.themedIcon().value_or(m_ext.themedIcon())),
                                                {20, 20})
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

QWidget *VicinaeStoreDetailView::createSideMetadataSection() {
  auto viewSource = VStack()
                        .addText("Source Code", SemanticColor::TextMuted)
                        .add(new TextLinkWidget("View Code", QUrl(m_ext.sourceUrl)))
                        .spacing(5);

  auto lastUpdate = VStack()
                        .addText("Last update", SemanticColor::TextMuted)
                        .addText(getRelativeTimeString(m_ext.updatedAt), SemanticColor::Foreground)
                        .spacing(5);

  auto categories = VStack().addText("Categories", SemanticColor::TextMuted).spacing(5);

  for (const auto &category : m_ext.categories) {
    categories.addText(category.name, SemanticColor::Foreground);
  }

  return VStack()
      .addIf(!m_ext.readmeUrl.isEmpty(),
             [&]() {
               return VStack()
                   .addText("README", SemanticColor::TextMuted)
                   .add(new TextLinkWidget("Open README", QUrl(m_ext.readmeUrl)))
                   .spacing(5);
             })
      .add(lastUpdate)
      .addIf(!m_ext.categories.empty(), [&]() { return categories; })
      .add(viewSource)
      .addStretch()
      .spacing(15)
      .margins(10)
      .buildWidget();
}

QWidget *VicinaeStoreDetailView::createContentSection() {
  return HStack()
      .add(createMainWidget().buildWidget(), 2)
      .add(createSideMetadataSection(), 1)
      .divided(1)
      .buildWidget();
}

void VicinaeStoreDetailView::createActions() {
  auto panel = std::make_unique<ActionPanelState>();
  auto registry = context()->services->extensionRegistry();
  bool isInstalled = registry->isInstalled(m_ext.id);
  auto main = panel->createSection();

  if (!isInstalled) {
    auto install = new StaticAction(
        "Install extension", m_ext.themedIcon(), [ext = m_ext](const ApplicationContext *ctx) {
          using Watcher = QFutureWatcher<VicinaeStore::DownloadExtensionResult>;
          auto store = ctx->services->vicinaeStore();
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

            registry->installFromZip(QString("store.vicinae.%1").arg(ext.name), result->toStdString(),
                                     [toast](bool ok) {
                                       if (!ok) {
                                         toast->failure("Failed to extract extension archive");
                                         return;
                                       }
                                       toast->success("Extension installed");
                                     });
          });

          auto downloadResult = store->downloadExtension(ext.downloadUrl);
          watcher->setFuture(downloadResult);
        });
    main->addAction(install);
  } else {
    auto uninstall = new UninstallExtensionAction(m_ext.id);
    main->addAction(uninstall);
  }

  setActions(std::move(panel));
}

void VicinaeStoreDetailView::initialize() {
  auto registry = context()->services->extensionRegistry();
  bool isInstalled = registry->isInstalled(m_ext.id);

  if (isInstalled) {
    extensionInstalled();
  } else {
    extensionUninstalled();
  }

  createActions();
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

bool VicinaeStoreDetailView::supportsSearch() const { return false; }

QString VicinaeStoreDetailView::initialNavigationTitle() const {
  return QString("%1 - %2").arg(BaseView::initialNavigationTitle()).arg(m_ext.title);
}

QWidget *VicinaeStoreDetailView::createUI(const VicinaeStore::Extension &ext) {
  return VStack()
      .add(createPresentationSection())
      .add(createContentSection())
      .addStretch()
      .divided(1)
      .buildWidget();
}

void VicinaeStoreDetailView::setupUI(const VicinaeStore::Extension &extension) {
  m_scrollArea->setWidget(createUI(extension));
  m_scrollArea->setFocusPolicy(Qt::StrongFocus);
  VStack().add(m_scrollArea).imbue(this);
}
