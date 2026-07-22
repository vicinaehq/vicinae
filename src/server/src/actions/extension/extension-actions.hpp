#pragma once
#include <QCoreApplication>
#include <qstring.h>
#include "ui/action-pannel/action.hpp"
#include "ui/image/url.hpp"

class UninstallExtensionAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(UninstallExtensionAction)

  QString m_id;

  std::optional<ImageURL> icon() const override { return ImageURL::builtin("computer-chip"); }
  void execute(ApplicationContext *ctx) override;
  QString title() const override { return tr("Uninstall Extension"); }

public:
  UninstallExtensionAction(const QString &id) : m_id(id) { setStyle(AbstractAction::Style::Danger); }
};
