#pragma once
#include "url.hpp"
#include "proto/ui.pb.h"

struct ProtoUrl {
  static ImageURL parse(const proto::ext::ui::Image &image) {
    using Source = proto::ext::ui::ImageSource;
    ExtensionImageModel model;

    if (image.has_tint_color()) {
      const auto &colorLike = image.tint_color();
      switch (colorLike.payload_case()) {
      case proto::ext::ui::ColorLike::kRaw: {
        QString raw = colorLike.raw().c_str();
        if (auto tint = ImageURL::tintForName(raw); tint != SemanticColor::InvalidTint) {
          model.tintColor = tint;
        } else {
          model.tintColor = raw;
        }
        break;
      }
      case proto::ext::ui::ColorLike::kDynamic: {
        const auto &dynamic = colorLike.dynamic();
        DynamicColor dynamicColor{.light = dynamic.light().c_str(),
                                  .dark = dynamic.dark().c_str(),
                                  .adjustContrast =
                                      dynamic.has_adjust_contrast() ? dynamic.adjust_contrast() : true};
        model.tintColor = dynamicColor;
        break;
      }
      default:
        break;
      }
    }

    if (image.has_mask()) {
      switch (image.mask()) {
      case proto::ext::ui::ImageMask::Circle:
        model.mask = OmniPainter::ImageMaskType::CircleMask;
        break;
      case proto::ext::ui::ImageMask::RoundedRectangle:
        model.mask = OmniPainter::ImageMaskType::RoundedRectangleMask;
        break;
      default:
        break;
      }
    }

    switch (image.source().payload_case()) {
    case Source::kRaw:
      model.source = image.source().raw().c_str();
      break;
    case Source::kThemed: {
      auto &themed = image.source().themed();
      model.source = ThemedIconSource{.light = themed.light().c_str(), .dark = themed.dark().c_str()};
      break;
    }
    default:
      break;
    }

    if (image.has_fallback()) {
      switch (image.fallback().payload_case()) {
      case Source::kRaw:
        model.fallback = image.fallback().raw().c_str();
        break;
      case Source::kThemed: {
        auto &themed = image.fallback().themed();
        model.fallback = ThemedIconSource{.light = themed.light().c_str(), .dark = themed.dark().c_str()};
        break;
      }
      default:
        break;
      }
    }

    return ImageLikeModel(model);
  }
};
