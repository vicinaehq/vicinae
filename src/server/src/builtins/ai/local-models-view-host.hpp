#pragma once
#ifdef HAS_LOCAL_AI
#include <algorithm>
#include <ranges>
#include <QCoreApplication>
#include <QTimer>
#include "actions/clipboard-actions.hpp"
#include "common/context.hpp"
#include "fuzzy/fuzzy-searchable.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "services/ai/ai-capability.hpp"
#include "services/local-model-registry/local-model-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "theme/colors.hpp"
#include "ui/action-panel/action-panel-state.hpp"
#include "ui/action-panel/action.hpp"
#include "ui/views/list-accessory.hpp"
#include "ui/views/mono-list-view-host.hpp"
#include "utils/utils.hpp"

template <> struct fuzzy::FuzzySearchable<LocalModel> {
  static fuzzy::Match score(const LocalModel &model, const fuzzy::Query &query) {
    return fuzzy::scoreWeighted({{std::string(model.info.name), 1.0}, {std::string(model.info.id), 0.6}},
                                query);
  }
};

class LocalModelsViewHost : public MonoListViewHost<LocalModel> {
  Q_DECLARE_TR_FUNCTIONS(LocalModelsViewHost)

public:
  explicit LocalModelsViewHost(std::optional<AI::Capabilities> filter = std::nullopt) : m_filter(filter) {
    m_refreshTimer.setSingleShot(true);
    m_refreshTimer.setInterval(REFRESH_INTERVAL_MS);
    connect(&m_refreshTimer, &QTimer::timeout, this, [this]() {
      notifyChanged();
      refreshDetail();
    });
  }

  void onMount() override {
    setNavigationTitle(tr("Local Models"));
    setSearchPlaceholderText(tr("Search models..."));

    auto *registry = this->registry();
    connect(registry, &LocalModelRegistry::modelsChanged, this, [this]() { reload(); });
    connect(registry, &LocalModelRegistry::downloadProgress, this, [this]() { scheduleRefresh(); });
    connect(registry, &LocalModelRegistry::downloadRetryScheduled, this, [this]() { scheduleRefresh(); });

    reload();
  }

  QString displayTitle(const ItemType &model) const override { return QString::fromUtf8(model.info.name); }

  QString displaySubtitle(const ItemType &) const override { return {}; }

  QString displayId(const ItemType &model) const override { return QString::fromUtf8(model.info.id); }

  std::optional<ImageURL> displayIcon(const ItemType &model) const override {
    return LocalModelCatalogue::vendorIcon(model.info.vendor);
  }

  AccessoryList displayAccessories(const ItemType &model) const override {
    if (auto *download = registry()->activeDownload(model.info.id)) return {downloadAccessory(*download)};
    if (model.installed) {
      return {
          ListAccessory{.tooltip = tr("Installed"),
                        .icon = ImageURL::builtin(BuiltinIcon::CheckCircle).setFill(SemanticColor::Green)}};
    }
    return {};
  }

  std::unique_ptr<ActionPanelState> buildActionPanel(const ItemType &model) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto *section = panel->createSection();
    const auto id = std::string(model.info.id);
    const auto name = QString::fromUtf8(model.info.name);

    if (registry()->activeDownload(id)) {
      section->addAction(new StaticAction(
          tr("Cancel Download"), ImageURL::builtin(BuiltinIcon::XMarkCircleFilled),
          [id](ApplicationContext *ctx) { ctx->services->localModels()->cancelDownload(id); }));
      return panel;
    }

    if (model.installed) {
      const auto path = QString::fromStdString(registry()->pathFor(model.info).string());
      const auto folder = QString::fromStdString(registry()->modelsDir().string());

      section->addAction(new StaticAction(
          tr("Show in Folder"), ImageURL::builtin(BuiltinIcon::Folder),
          [folder](ApplicationContext *ctx) { ctx->services->appDb()->openTarget(folder); }));
      section->addAction(new CopyToClipboardAction(Clipboard::Text(path), tr("Copy Path")));

      auto *remove = new StaticAction(
          tr("Delete Model"), ImageURL::builtin(BuiltinIcon::Trash), [id, name](ApplicationContext *ctx) {
            ctx->navigation->confirmAlert(
                tr("Delete %1?").arg(name), tr("The model file will be removed from disk."), [ctx, id]() {
                  if (auto result = ctx->services->localModels()->remove(id); !result) {
                    ctx->services->toastService()->failure(tr("Could not delete model"),
                                                           QString::fromStdString(result.error()));
                  }
                });
          });
      remove->setStyle(AbstractAction::Style::Danger);
      panel->createSection()->addAction(remove);
      return panel;
    }

    section->addAction(
        new StaticAction(tr("Download"), ImageURL::builtin(BuiltinIcon::Download),
                         [id, name](ApplicationContext *ctx) { startDownload(ctx, id, name); }));
    section->addAction(new CopyToClipboardAction(
        Clipboard::Text(registry()->downloadUrl(model.info).toString()), tr("Copy Download URL")));
    return panel;
  }

protected:
  bool hasDetailPane() const override { return true; }

  std::optional<ListItemDetail> displayDetail(const ItemType &model) const override {
    const auto &info = model.info;
    QVariantList meta;

    auto row = [&meta](const QString &label, const QString &value) {
      meta.append(QVariantMap{{QStringLiteral("label"), label}, {QStringLiteral("value"), value}});
    };

    row(tr("Status"), statusText(model));
    row(tr("Vendor"), vendorName(info.vendor));
    if (info.languages) {
      row(tr("Languages"),
          QCoreApplication::translate(LocalModelCatalogue::TRANSLATION_CONTEXT, info.languages));
    }
    row(tr("Precision"), QString::fromUtf8(info.quantization));
    row(tr("Size"), formatSize(info.size));
    row(tr("File"), QString::fromUtf8(info.file));
    row(tr("Source"), QString::fromUtf8(info.repo));

    return ListItemDetail{.metadata = std::move(meta),
                          .markdown = LocalModelCatalogue::translatedDescription(info)};
  }

  void sortFiltered() override {
    if (!m_query.empty()) return;
    std::ranges::stable_sort(m_filtered, [this](const auto &a, const auto &b) {
      return rank(m_items[a.data]) < rank(m_items[b.data]);
    });
  }

private:
  static constexpr int REFRESH_INTERVAL_MS = 150;

  static void startDownload(ApplicationContext *ctx, const std::string &id, const QString &name) {
    auto *toast = ctx->services->toastService();
    auto result = ctx->services->localModels()->download(id);

    if (!result) {
      toast->failure(tr("Could not start download"), QString::fromStdString(result.error()));
      return;
    }

    auto *download = *result;
    toast->dynamic(tr("Downloading %1…").arg(name));

    connect(download, &ModelDownload::progress, toast, [toast, name](qint64 received, qint64 total) {
      if (total > 0) toast->dynamic(tr("Downloading %1… %2%").arg(name).arg(received * 100 / total));
    });
    connect(download, &ModelDownload::retryScheduled, toast,
            [toast, name](int, int delaySeconds, const QString &) {
              toast->dynamic(tr("Retrying %1 in %n second(s)…", nullptr, delaySeconds).arg(name));
            });
    connect(download, &ModelDownload::finished, toast,
            [toast, name]() { toast->success(tr("%1 installed").arg(name)); });
    connect(download, &ModelDownload::failed, toast, [toast, name](const QString &error) {
      toast->failure(tr("Could not download %1").arg(name), error);
    });
    connect(download, &ModelDownload::cancelled, toast, [toast]() { toast->clear(); });
  }

  QString statusText(const LocalModel &model) const {
    if (auto *download = registry()->activeDownload(model.info.id)) return downloadAccessory(*download).text;
    return model.installed ? tr("Installed") : tr("Not installed");
  }

  static QString vendorName(LocalModelVendor vendor) {
    switch (vendor) {
    case LocalModelVendor::OpenAI:
      return QStringLiteral("OpenAI");
    case LocalModelVendor::Nvidia:
      return QStringLiteral("NVIDIA");
    case LocalModelVendor::Silero:
      return QStringLiteral("Silero");
    }
    return {};
  }

  static int rank(const LocalModel &model) {
    if (model.downloading) return 0;
    if (model.installed) return 1;
    return 2;
  }

  ListAccessory downloadAccessory(const ModelDownload &download) const {
    if (download.state() == ModelDownload::State::WaitingRetry) {
      return {.text = tr("Retrying…"), .color = SemanticColor::Orange};
    }
    if (download.state() == ModelDownload::State::Verifying) {
      return {.text = tr("Verifying…"), .color = SemanticColor::Blue};
    }
    if (download.bytesTotal() > 0) {
      const auto pct = static_cast<int>(download.bytesReceived() * 100 / download.bytesTotal());
      return {.text = tr("%1%").arg(pct), .color = SemanticColor::Blue};
    }
    return {.text = tr("Downloading…"), .color = SemanticColor::Blue};
  }

  LocalModelRegistry *registry() const { return context()->services->localModels(); }

  void scheduleRefresh() {
    if (!m_refreshTimer.isActive()) m_refreshTimer.start();
  }

  void reload() {
    auto models = registry()->models(m_filter);
    std::erase_if(models, [](const LocalModel &model) { return model.info.caps == 0; });
    setItems(std::move(models));
  }

  std::optional<AI::Capabilities> m_filter;
  QTimer m_refreshTimer;
};
#endif
