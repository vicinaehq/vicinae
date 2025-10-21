#pragma once
#include "command-database.hpp"
#include "ui/image/url.hpp"
#include "vicinae.hpp"
#include <qlogging.h>

class ClipboardExtension : public BuiltinCommandRepository {
public:
  QString id() const override { return "clipboard"; }
  QString displayName() const override { return "Clipboard"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("copy-clipboard").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  QString description() const override { return "System clipboard integration"; }

  std::vector<Preference> preferences() const override;
  virtual void initialized(const QJsonObject &preferences) const override;
  void preferenceValuesChanged(const QJsonObject &value) const override;

  ClipboardExtension();
};
