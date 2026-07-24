#pragma once
#include <QCoreApplication>
#include "command-database.hpp"
#include "ui/image/url.hpp"
#include "vicinae.hpp"
#include <qlogging.h>

class ClipboardExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(ClipboardExtension)

public:
  QString id() const override { return "clipboard"; }
  QString displayName() const override { return tr("Clipboard"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("copy-clipboard").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  QString description() const override { return tr("System clipboard integration"); }

  std::vector<Preference> preferences() const override;
  virtual void initialized(const QJsonObject &preferences) const override;
  void preferenceValuesChanged(const QJsonObject &value) const override;

  ClipboardExtension();
};
