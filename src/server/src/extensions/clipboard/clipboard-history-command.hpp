#pragma once
#include "history/clipboard-history-view.hpp"
#include "single-view-command-context.hpp"
#include "vicinae.hpp"
#include <qjsonobject.h>

class ClipboardHistoryCommand : public BuiltinViewCommand<ClipboardHistoryView> {
  QString id() const override { return "history"; }
  QString name() const override { return "Clipboard History"; }
  QString description() const override {
    return "Browse your clipboard's history, pin, edit and remove entries.";
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("copy-clipboard").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  std::vector<Preference> preferences() const override;
  void preferenceValuesChanged(const QJsonObject &value) const override;
};
