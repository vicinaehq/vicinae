#pragma once
#include <QCoreApplication>
#include "builtin_icon.hpp"
#include "common/context.hpp"
#include "fuzzy/fuzzy-searchable.hpp"
#include "mono-list-view-host.hpp"
#include "services/media-control/media-control-service.hpp"
#include "theme/colors.hpp"
#include "ui/action-pannel/action-panel-state.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/list-accessory/list-accessory.hpp"

template <> struct fuzzy::FuzzySearchable<MediaPlayer> {
  static fuzzy::Match score(const MediaPlayer &e, const fuzzy::Query &query) {
    return fuzzy::scoreWeighted(
        {{e.title.toStdString(), 1.0}, {e.artist.toStdString(), 0.8}, {e.identity.toStdString(), 0.6}},
        query);
  }
};

class NowPlayingViewHost : public MonoListViewHost<MediaPlayer> {
  Q_DECLARE_TR_FUNCTIONS(NowPlayingViewHost)

public:
  void onMount() override {
    setSearchPlaceholderText(tr("Search players..."));

    auto media = context()->services->mediaControl()->provider();

    connect(media, &AbstractMediaControl::playersChanged, this, [this]() { reload(); });
    reload();
  }

  QString sectionName() const override { return tr("Players"); }

  QString displayId(const ItemType &e) const override { return e.id; }

  QString displayTitle(const ItemType &e) const override { return e.title.isEmpty() ? e.identity : e.title; }

  QString displaySubtitle(const ItemType &e) const override { return e.artist; }

  std::optional<ImageURL> displayIcon(const ItemType &e) const override {
    if (e.status == PlaybackStatus::Playing) {
      return ImageURL::builtin(BuiltinIcon::PauseFilled).setFill(SemanticColor::Green);
    }

    return ImageURL::builtin(BuiltinIcon::PlayFilled);
  }

  AccessoryList displayAccessories(const ItemType &e) const override {
    if (e.title.isEmpty()) return {};

    return {ListAccessory{.text = e.identity}};
  }

  std::unique_ptr<ActionPanelState> buildActionPanel(const ItemType &e) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto main = panel->createSection();
    const bool playing = e.status == PlaybackStatus::Playing;

    main->addAction(new StaticAction(
        playing ? tr("Pause") : tr("Play"),
        ImageURL::builtin(playing ? BuiltinIcon::Pause : BuiltinIcon::Play),
        [id = e.id](ApplicationContext *ctx) { ctx->services->mediaControl()->provider()->playPause(id); }));

    if (e.canGoNext) {
      main->addAction(new StaticAction(
          tr("Next Track"), ImageURL::builtin(BuiltinIcon::Forward),
          [id = e.id](ApplicationContext *ctx) { ctx->services->mediaControl()->provider()->next(id); }));
    }

    if (e.canGoPrevious) {
      main->addAction(new StaticAction(
          tr("Previous Track"), ImageURL::builtin(BuiltinIcon::Rewind),
          [id = e.id](ApplicationContext *ctx) { ctx->services->mediaControl()->provider()->previous(id); }));
    }

    return panel;
  }

private:
  void reload() { setItems(context()->services->mediaControl()->provider()->players()); }
};
