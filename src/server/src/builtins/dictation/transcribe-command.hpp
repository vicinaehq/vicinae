#pragma once
#include <vector>
#include <QCoreApplication>
#include "command/command-database.hpp"
#include "command/single-view-command-context.hpp"
#include "ui/image/url.hpp"

class TranscribeCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(TranscribeCommand)

  QString id() const override { return "transcribe"; }
  QString name() const override { return tr("Transcribe"); }
  ImageURL iconUrl() const override;
  std::vector<QString> keywords() const override { return {"dictate"}; }

  void execute(CommandController &controller) const override;
};
