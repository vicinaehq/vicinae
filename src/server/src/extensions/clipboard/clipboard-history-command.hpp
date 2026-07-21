#pragma once
#include <QCoreApplication>
#include "qml/clipboard-history-view-host.hpp"
#include "single-view-command-context.hpp"
#include "vicinae.hpp"
#include <qjsonobject.h>

class ClipboardHistoryCommand : public BuiltinViewCommand<ClipboardHistoryViewHost> {
  Q_DECLARE_TR_FUNCTIONS(ClipboardHistoryCommand)
  QString id() const override { return "history"; }
  QString name() const override { return tr("Clipboard History"); }
  QString description() const override {
    return tr("Browse your clipboard's history, pin, edit and remove entries.");
  }
  std::vector<QString> keywords() const override { return {"Clipboard History"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("copy-clipboard").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  std::vector<Preference> preferences() const override;
  void preferenceValuesChanged(const QJsonObject &value) const override;
};
