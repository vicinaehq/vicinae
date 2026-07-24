#include "command-database.hpp"
#include <QCoreApplication>

class PowerManagementExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(PowerManagementExtension)

  QString id() const override { return "power"; }
  QString displayName() const override { return tr("Power Management"); }
  QString description() const override { return tr("Power off, suspend, sleep, hibernate your computer."); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("power").setBackgroundTint(SemanticColor::Red);
  }

public:
  PowerManagementExtension();
};
