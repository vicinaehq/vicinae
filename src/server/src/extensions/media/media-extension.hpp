#pragma once
#include <QCoreApplication>
#include "command-database.hpp"
#include "common/context.hpp"
#include "services/audio-control/audio-control-service.hpp"
#include "services/toast/toast-service.hpp"
#include "single-view-command-context.hpp"
#include "theme/colors.hpp"

#ifdef Q_OS_LINUX
#include "qml/now-playing-view-host.hpp"
#include "services/media-control/media-control-service.hpp"
#endif

namespace {

const QColor MEDIA_COMMAND_TINT = QColor(128, 132, 138);

#ifdef Q_OS_LINUX

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

#endif

ImageURL volumeIcon(float level) {
  if (level <= 0.0f) return ImageURL{BuiltinIcon::SpeakerOff};
  if (level <= 0.33f) return ImageURL{BuiltinIcon::SpeakerLow};
  if (level <= 0.66f) return ImageURL{BuiltinIcon::SpeakerDown};
  return ImageURL{BuiltinIcon::SpeakerHigh};
}

void showVolumeHud(const ApplicationContext *ctx, float level) {
  auto vol = qRound(level * 100);
  ctx->navigation->showHud(QCoreApplication::translate("media-extension", "Volume %1%").arg(vol),
                           volumeIcon(level));
}

class VolumeUpCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(VolumeUpCommand)

  QString id() const override { return "volume-up"; }
  QString name() const override { return tr("Turn Volume Up"); }
  QString description() const override { return tr("Increase system volume"); }
  std::vector<QString> keywords() const override { return {"audio", "sound", "louder"}; }
  ImageURL iconUrl() const override {
    return ImageURL{BuiltinIcon::SpeakerUp}.setBackgroundTint(MEDIA_COMMAND_TINT);
  }
  std::vector<CommandArgument> arguments() const override {
    return {{.name = "step", .type = CommandArgument::Text, .placeholder = "+5", .required = false}};
  }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();
    auto audio = ctx->services->audioControl()->provider();
    auto arg = controller.launchProps().arguments;
    int step = 5;
    if (!arg.empty() && !arg.front().second.isEmpty()) {
      bool ok = false;
      step = arg.front().second.toInt(&ok);
      if (!ok) {
        ctx->services->toastService()->failure(tr("Invalid step value"));
        return;
      }
    }
    auto result = audio->adjustVolume(static_cast<float>(step) / 100.0f);
    if (!result) {
      ctx->services->toastService()->failure(tr("Failed to adjust volume"));
      return;
    }
    showVolumeHud(ctx, *result);
  }
};

class VolumeDownCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(VolumeDownCommand)

  QString id() const override { return "volume-down"; }
  QString name() const override { return tr("Turn Volume Down"); }
  QString description() const override { return tr("Decrease system volume"); }
  std::vector<QString> keywords() const override { return {"audio", "sound", "quieter"}; }
  ImageURL iconUrl() const override {
    return ImageURL{BuiltinIcon::SpeakerDown}.setBackgroundTint(MEDIA_COMMAND_TINT);
  }
  std::vector<CommandArgument> arguments() const override {
    return {{.name = "step", .type = CommandArgument::Text, .placeholder = "-5", .required = false}};
  }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();
    auto audio = ctx->services->audioControl()->provider();
    auto arg = controller.launchProps().arguments;
    int step = -5;
    if (!arg.empty() && !arg.front().second.isEmpty()) {
      bool ok = false;
      step = arg.front().second.toInt(&ok);
      if (!ok) {
        ctx->services->toastService()->failure(tr("Invalid step value"));
        return;
      }
    }
    auto result = audio->adjustVolume(static_cast<float>(step) / 100.0f);
    if (!result) {
      ctx->services->toastService()->failure(tr("Failed to adjust volume"));
      return;
    }
    showVolumeHud(ctx, *result);
  }
};

template <int Percent, BuiltinIcon Icon> class SetVolumeCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(SetVolumeCommand)

  static constexpr float LEVEL = static_cast<float>(Percent) / 100.0f;

  QString id() const override { return "volume-" + QString::number(Percent); }
  QString name() const override { return tr("Set Volume to %1%").arg(Percent); }
  QString description() const override { return tr("Set system volume to %1%").arg(Percent); }
  std::vector<QString> keywords() const override { return {"audio", "sound", "volume"}; }
  ImageURL iconUrl() const override { return ImageURL{Icon}.setBackgroundTint(MEDIA_COMMAND_TINT); }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();
    auto audio = ctx->services->audioControl()->provider();
    auto result = audio->setVolume(LEVEL);
    if (!result) {
      ctx->services->toastService()->failure(tr("Failed to set volume"));
      return;
    }
    showVolumeHud(ctx, *result);
  }
};

class ToggleMuteCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(ToggleMuteCommand)

  QString id() const override { return "toggle-mute"; }
  QString name() const override { return tr("Toggle Mute"); }
  QString description() const override { return tr("Mute or unmute system audio"); }
  std::vector<QString> keywords() const override { return {"audio", "sound", "volume", "mute", "unmute"}; }
  ImageURL iconUrl() const override {
    return ImageURL{BuiltinIcon::SpeakerOff}.setBackgroundTint(MEDIA_COMMAND_TINT);
  }

  void execute(CommandController &controller) const override {
    auto ctx = controller.context();
    auto audio = ctx->services->audioControl()->provider();
    if (!audio->toggleMute()) {
      ctx->services->toastService()->failure(tr("Failed to toggle mute"));
      return;
    }
    if (audio->isMuted()) {
      ctx->navigation->showHud(tr("Muted"), ImageURL{BuiltinIcon::SpeakerOff});
    } else {
      showVolumeHud(ctx, audio->getVolume());
    }
  }
};

} // namespace

class MediaExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(MediaExtension)

  QString id() const override { return "media"; }
  QString displayName() const override { return tr("Media"); }
  QString description() const override { return tr("Control media playback and system audio"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Music).setBackgroundTint(MEDIA_COMMAND_TINT);
  }

public:
  MediaExtension() {
    // only MPRIS is implemented for now, other platforms get the dummy backend
#ifdef Q_OS_LINUX
    registerCommand<NowPlayingCommand>();
    registerCommand<PlayPauseCommand>();
    registerCommand<NextTrackCommand>();
    registerCommand<PreviousTrackCommand>();
#endif
    registerCommand<VolumeUpCommand>();
    registerCommand<VolumeDownCommand>();
    registerCommand<SetVolumeCommand<100, BuiltinIcon::SpeakerHigh>>();
    registerCommand<SetVolumeCommand<75, BuiltinIcon::SpeakerHigh>>();
    registerCommand<SetVolumeCommand<50, BuiltinIcon::SpeakerLow>>();
    registerCommand<SetVolumeCommand<25, BuiltinIcon::SpeakerLow>>();
    registerCommand<SetVolumeCommand<0, BuiltinIcon::SpeakerOff>>();
    registerCommand<ToggleMuteCommand>();
  }
};
