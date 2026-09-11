#pragma once
#include "services/builtin-icon/builtin-icon.hpp"
#include "command/command-database.hpp"
#include "command/single-view-command-context.hpp"
#include "builtins/dictation/transcribe-view-host.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"

namespace {
const auto DICTATION_ICON = ImageURL::builtin(BuiltinIcon::Microphone).setBackgroundTint(SemanticColor::Blue);

class TranscribeCommand : public BuiltinViewCommand<TranscribeViewHost> {
  Q_DECLARE_TR_FUNCTIONS(TranscribeCommand)

  QString id() const override { return "transcribe"; }
  QString name() const override { return tr("Transcribe"); }
  ImageURL iconUrl() const override { return DICTATION_ICON; }
  std::vector<QString> keywords() const override { return {"dictate"}; }
};
}; // namespace

class DictationExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(DictationExtension)

  QString id() const override { return "dictation"; }
  QString displayName() const override { return tr("Dictation"); }
  ImageURL iconUrl() const override { return DICTATION_ICON; }

public:
  DictationExtension() { registerCommand<TranscribeCommand>(); }
};
