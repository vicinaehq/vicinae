#include "command-database.hpp"

class PowerManagementExtension : public BuiltinCommandRepository {
  QString id() const override { return "power"; }
  QString displayName() const override { return "Power Management"; }
  QString description() const override { return "Power off, suspend, sleep, hibernate your computer."; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("power").setBackgroundTint(SemanticColor::Red);
  }

public:
  PowerManagementExtension();
};
