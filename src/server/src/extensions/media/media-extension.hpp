#pragma once
#include <QCoreApplication>
#include "command-database.hpp"
#include "common/context.hpp"
#include "qml/now-playing-view-host.hpp"
#include "services/media-control/media-control-service.hpp"
#include "services/toast/toast-service.hpp"
#include "single-view-command-context.hpp"
#include "theme/colors.hpp"

namespace {

constexpr auto MEDIA_COMMAND_TINT = SemanticColor::Magenta;

QString playerQuery(const CommandController &controller) {
  const auto &args = controller.launchProps().arguments;

  return args.empty() ? QString{} : args.front().second;
}

QString trackLabel(const MediaPlayer &player) {
  if (player.title.isEmpty()) return player.identity;
  if (player.artist.isEmpty()) return player.title;

  return QCoreApplication::translate("media-extension", "%1 — %2").arg(player.title, player.artist);
}

std::optional<MediaPlayer> findPlayer(const AbstractMediaControl *media, const QString &query) {
  if (query.isEmpty()) return media->defaultPlayer();

  auto all = media->players();
  const auto text = query.toStdString();
  std::vector<Scored<int>> filtered;

  fuzzy::fuzzyFilter<MediaPlayer>(all, text, filtered);

  if (filtered.empty()) return std::nullopt;

  return std::move(all[filtered.front().data]);
}

/**
 * Player the command should act on, notifying the user when there is none.
 */
std::optional<MediaPlayer> resolvePlayer(const ApplicationContext *ctx, const QString &query) {
  auto player = findPlayer(ctx->services->mediaControl()->provider(), query);

  if (player) return player;

  ctx->services->toastService()->failure(
      query.isEmpty()
          ? QCoreApplication::translate("media-extension", "No media player is running")
          : QCoreApplication::translate("media-extension", "No media player matches \"%1\"").arg(query));

  return std::nullopt;
}

std::vector<CommandArgument> playerArgument(const QString &placeholder) {
  return {{.name = "player", .type = CommandArgument::Text, .placeholder = placeholder, .required = false}};
}

class PlayPauseCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(PlayPauseCommand)

  QString id() const override { return "play-pause"; }
  QString name() const override { return tr("Play / Pause"); }
  QString description() const override { return tr("Toggle playback of the active media player"); }
  std::vector<QString> keywords() const override { return {"media", "music", "play", "pause", "resume"}; }
  ImageURL iconUrl() const override {
    return ImageURL{BuiltinIcon::Play}.setBackgroundTint(MEDIA_COMMAND_TINT);
  }
  std::vector<CommandArgument> arguments() const override { return playerArgument(tr("player")); }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();
    const auto player = resolvePlayer(ctx, playerQuery(controller));

    if (!player) return;

    if (!ctx->services->mediaControl()->provider()->playPause(player->id)) {
      ctx->services->toastService()->failure(tr("Failed to toggle playback"));
      return;
    }

    if (player->status == PlaybackStatus::Playing) {
      ctx->navigation->showHud(tr("Paused"), ImageURL{BuiltinIcon::Pause});
    } else {
      ctx->navigation->showHud(tr("Playing %1").arg(trackLabel(*player)), ImageURL{BuiltinIcon::Play});
    }
  }
};

class NextTrackCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(NextTrackCommand)

  QString id() const override { return "next-track"; }
  QString name() const override { return tr("Next Track"); }
  QString description() const override { return tr("Skip to the next track"); }
  std::vector<QString> keywords() const override { return {"media", "music", "skip", "forward"}; }
  ImageURL iconUrl() const override {
    return ImageURL{BuiltinIcon::Forward}.setBackgroundTint(MEDIA_COMMAND_TINT);
  }
  std::vector<CommandArgument> arguments() const override { return playerArgument(tr("player")); }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();
    const auto player = resolvePlayer(ctx, playerQuery(controller));

    if (!player) return;

    auto toast = ctx->services->toastService();

    if (!player->canGoNext) {
      toast->failure(tr("%1 cannot skip to the next track").arg(player->identity));
      return;
    }

    if (!ctx->services->mediaControl()->provider()->next(player->id)) {
      toast->failure(tr("Failed to skip to the next track"));
      return;
    }

    ctx->navigation->showHud(tr("Next Track"), ImageURL{BuiltinIcon::Forward});
  }
};

class PreviousTrackCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(PreviousTrackCommand)

  QString id() const override { return "previous-track"; }
  QString name() const override { return tr("Previous Track"); }
  QString description() const override { return tr("Skip to the previous track"); }
  std::vector<QString> keywords() const override { return {"media", "music", "back", "rewind"}; }
  ImageURL iconUrl() const override {
    return ImageURL{BuiltinIcon::Rewind}.setBackgroundTint(MEDIA_COMMAND_TINT);
  }
  std::vector<CommandArgument> arguments() const override { return playerArgument(tr("player")); }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();
    const auto player = resolvePlayer(ctx, playerQuery(controller));

    if (!player) return;

    auto toast = ctx->services->toastService();

    if (!player->canGoPrevious) {
      toast->failure(tr("%1 cannot skip to the previous track").arg(player->identity));
      return;
    }

    if (!ctx->services->mediaControl()->provider()->previous(player->id)) {
      toast->failure(tr("Failed to skip to the previous track"));
      return;
    }

    ctx->navigation->showHud(tr("Previous Track"), ImageURL{BuiltinIcon::Rewind});
  }
};

class NowPlayingCommand : public BuiltinViewCommand<NowPlayingViewHost> {
  Q_DECLARE_TR_FUNCTIONS(NowPlayingCommand)

  QString id() const override { return "now-playing"; }
  QString name() const override { return tr("Now Playing"); }
  QString description() const override { return tr("Browse and control running media players"); }
  std::vector<QString> keywords() const override { return {"media", "music", "player", "mpris"}; }
  ImageURL iconUrl() const override {
    return ImageURL{BuiltinIcon::Music}.setBackgroundTint(MEDIA_COMMAND_TINT);
  }
};

} // namespace

class MediaExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(MediaExtension)

  QString id() const override { return "media"; }
  QString displayName() const override { return tr("Media"); }
  QString description() const override { return tr("Control running media players"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Music).setBackgroundTint(MEDIA_COMMAND_TINT);
  }

public:
  MediaExtension() {
    registerCommand<NowPlayingCommand>();
    registerCommand<PlayPauseCommand>();
    registerCommand<NextTrackCommand>();
    registerCommand<PreviousTrackCommand>();
  }
};
