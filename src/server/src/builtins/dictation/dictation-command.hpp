#pragma once
#include <vector>
#include <QCoreApplication>
#include "builtins/dictation/dictation-preferences.hpp"
#include "command/command-database.hpp"
#include "command/single-view-command-context.hpp"
#include "command/typed-command.hpp"
#include "ui/image/url.hpp"

class TranscribeCommand : public TypedCallbackCommand<NoPreferences, DictationPreferences> {
  Q_DECLARE_TR_FUNCTIONS(TranscribeCommand)

  QString id() const override { return "dictate"; }
  QString name() const override { return tr("Dictate"); }
  ImageURL iconUrl() const override;
  std::vector<QString> keywords() const override { return {"transcribe"}; }

  void execute(const Controller &controller) const override;
  void shortcutReleased() const override;
};
