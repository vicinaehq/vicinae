#pragma once
#include <QCoreApplication>
#include "builtins/clipboard/clipboard-preferences.hpp"
#include "command/typed-command.hpp"
#include "ui/image/url.hpp"
#include "vicinae.hpp"

class ClipboardExtension : public TypedCommandRepository<ClipboardPreferences> {
  Q_DECLARE_TR_FUNCTIONS(ClipboardExtension)

public:
  QString id() const override { return "clipboard"; }
  QString displayName() const override { return tr("Clipboard"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::CopyClipboard).setBackgroundTint(SemanticColor::Red);
  }
  QString description() const override { return tr("System clipboard integration"); }

  void initialized(const ClipboardPreferences &preferences) const override;
  void preferencesChanged(const ClipboardPreferences &preferences) const override;

  ClipboardExtension();
};
