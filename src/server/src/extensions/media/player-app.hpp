#pragma once
#include <memory>
#include <vector>
#include "common/context.hpp"
#include "services/app-service/app-service.hpp"
#include "services/media-control/abstract-media-control.hpp"

inline std::shared_ptr<AbstractApplication> playerApp(const ApplicationContext *ctx,
                                                      const MediaPlayer &player) {
  if (player.appId.isEmpty()) return nullptr;
  return ctx->services->appDb()->find(player.appId);
}

inline void resolvePlayerIdentity(const ApplicationContext *ctx, MediaPlayer &player) {
  if (auto app = playerApp(ctx, player)) player.identity = app->displayName();
}

inline void resolvePlayerIdentities(const ApplicationContext *ctx, std::vector<MediaPlayer> &players) {
  for (auto &player : players) {
    resolvePlayerIdentity(ctx, player);
  }
}
