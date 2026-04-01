#pragma once
#include "generated/tsapi.hpp"
#include "ui/image/url.hpp"

struct TsapiImage {
  static ImageURL parse(const tsapi::Image &image) {
    ExtensionImageModel model;

    if (image.tintColor) {
      const auto &colorLike = *image.tintColor;
      if (colorLike.raw) {
        QString raw = QString::fromStdString(*colorLike.raw);
        if (auto tint = ImageURL::tintForName(raw); tint != SemanticColor::InvalidTint) {
          model.tintColor = tint;
        } else {
          model.tintColor = raw;
        }
      } else if (colorLike.dynamic) {
        DynamicColor dynamicColor{.light = QString::fromStdString(colorLike.dynamic->light),
                                  .dark = QString::fromStdString(colorLike.dynamic->dark),
                                  .adjustContrast = colorLike.dynamic->adjustContrast.value_or(true)};
        model.tintColor = dynamicColor;
      }
    }

    if (image.mask) {
      switch (*image.mask) {
      case tsapi::ImageMask::Circle:
        model.mask = OmniPainter::ImageMaskType::CircleMask;
        break;
      case tsapi::ImageMask::RoundedRectangle:
        model.mask = OmniPainter::ImageMaskType::RoundedRectangleMask;
        break;
      default:
        break;
      }
    }

    if (image.source.raw) {
      model.source = QString::fromStdString(*image.source.raw);
    } else if (image.source.themed) {
      model.source =
          ThemedIconSource{.light = QString::fromStdString(image.source.themed->light.value_or("")),
                           .dark = QString::fromStdString(image.source.themed->dark.value_or(""))};
    }

    if (image.fallback) {
      if (image.fallback->raw) {
        model.fallback = QString::fromStdString(*image.fallback->raw);
      } else if (image.fallback->themed) {
        model.fallback =
            ThemedIconSource{.light = QString::fromStdString(image.fallback->themed->light.value_or("")),
                             .dark = QString::fromStdString(image.fallback->themed->dark.value_or(""))};
      }
    }

    return ImageLikeModel(model);
  }
};
