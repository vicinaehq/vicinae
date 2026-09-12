#pragma once
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
#include "services/local-speech-model-registry/local-speech-model-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "theme/colors.hpp"
#include "ui/action-panel/action-panel-state.hpp"
#include "ui/action-panel/action.hpp"
#include "ui/views/list-accessory.hpp"
#include "ui/views/mono-list-view-host.hpp"
#include "utils/utils.hpp"

template <> struct fuzzy::FuzzySearchable<SpeechModel> {
  static fuzzy::Match score(const SpeechModel &model, const fuzzy::Query &query) {
    return fuzzy::scoreWeighted({{std::string(model.info.name), 1.0}, {std::string(model.info.id), 0.6}},
                                query);
  }
};

class DictationModelsViewHost : public MonoListViewHost<SpeechModel> {
  Q_DECLARE_TR_FUNCTIONS(DictationModelsViewHost)

public:
  DictationModelsViewHost() {
    m_refreshTimer.setSingleShot(true);
    m_refreshTimer.setInterval(REFRESH_INTERVAL_MS);
    connect(&m_refreshTimer, &QTimer::timeout, this, [this]() {
      notifyChanged();
      refreshDetail();
    });
  }

  void onMount() override {
    setNavigationTitle(tr("Dictation Models"));
    setSearchPlaceholderText(tr("Search models..."));

    auto *registry = this->registry();
    connect(registry, &LocalSpeechModelRegistry::modelsChanged, this, [this]() { reload(); });
    connect(registry, &LocalSpeechModelRegistry::downloadProgress, this, [this]() { scheduleRefresh(); });
    connect(registry, &LocalSpeechModelRegistry::downloadRetryScheduled, this,
            [this]() { scheduleRefresh(); });

    reload();
  }

  QString displayTitle(const ItemType &model) const override { return QString::fromUtf8(model.info.name); }

  QString displaySubtitle(const ItemType &) const override { return {}; }

  QString displayId(const ItemType &model) const override { return QString::fromUtf8(model.info.id); }

  std::optional<ImageURL> displayIcon(const ItemType &model) const override {
    return SpeechModelCatalogue::vendorIcon(model.info.vendor);
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
          [id](ApplicationContext *ctx) { ctx->services->speechModels()->cancelDownload(id); }));
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
                  if (auto result = ctx->services->speechModels()->remove(id); !result) {
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
    row(tr("Engine"), engineName(info.engine));
    row(tr("Vendor"), vendorName(info.vendor));
    row(tr("Languages"),
        QCoreApplication::translate(SpeechModelCatalogue::TRANSLATION_CONTEXT, info.languages));
    row(tr("Precision"), QString::fromUtf8(info.quantization));
    row(tr("Size"), formatSize(info.size));
    row(tr("File"), QString::fromUtf8(info.file));
    row(tr("Source"), QString::fromUtf8(info.repo));

    return ListItemDetail{.metadata = std::move(meta),
                          .markdown = SpeechModelCatalogue::translatedDescription(info)};
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
    auto result = ctx->services->speechModels()->download(id);

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

  QString statusText(const SpeechModel &model) const {
    if (auto *download = registry()->activeDownload(model.info.id)) return downloadAccessory(*download).text;
    return model.installed ? tr("Installed") : tr("Not installed");
  }

  static QString engineName(SpeechEngine engine) {
    switch (engine) {
    case SpeechEngine::Whisper:
      return QStringLiteral("Whisper");
    case SpeechEngine::Parakeet:
      return QStringLiteral("Parakeet");
    case SpeechEngine::Vad:
      return QStringLiteral("Silero VAD");
    }
    return {};
  }

  static QString vendorName(SpeechModelVendor vendor) {
    switch (vendor) {
    case SpeechModelVendor::OpenAI:
      return QStringLiteral("OpenAI");
    case SpeechModelVendor::Nvidia:
      return QStringLiteral("NVIDIA");
    case SpeechModelVendor::Silero:
      return QStringLiteral("Silero");
    }
    return {};
  }

  static int rank(const SpeechModel &model) {
    if (model.downloading) return 0;
    if (model.installed) return 1;
    if (model.info.recommended) return 2;
    return 3;
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

  LocalSpeechModelRegistry *registry() const { return context()->services->speechModels(); }

  void scheduleRefresh() {
    if (!m_refreshTimer.isActive()) m_refreshTimer.start();
  }

  void reload() {
    auto models = registry()->models();
    std::erase_if(models, [](const SpeechModel &model) { return model.info.engine == SpeechEngine::Vad; });
    setItems(std::move(models));
  }

  QTimer m_refreshTimer;
};
