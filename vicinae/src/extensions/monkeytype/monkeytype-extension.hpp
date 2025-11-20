#include "command-database.hpp"
#include "single-view-command-context.hpp"
#include "monkeytype/monkeytype-view.hpp"
#include "theme/colors.hpp"

class MonkeyTypeCommand : public BuiltinViewCommand<MonkeyTypeView> {
  QString id() const override { return "type"; }
  QString name() const override { return "Monkeytype"; }
  QString description() const override { return "Type like a monkey 🐒"; }
  ImageURL iconUrl() const override { return ImageURL::emoji("🐒").setBackgroundTint(SemanticColor::Yellow); }
};

class MonkeyTypeExtension : public BuiltinCommandRepository {
  QString id() const override { return "monkeytype"; }
  QString displayName() const override { return "Monkeytype"; }
  QString description() const override { return "Type like a monkey 🐒"; }
  ImageURL iconUrl() const override { return ImageURL::emoji("🐒").setBackgroundTint(SemanticColor::Yellow); }

public:
  MonkeyTypeExtension() { registerCommand<MonkeyTypeCommand>(); }
};
