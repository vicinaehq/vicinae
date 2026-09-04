#include "calculator-actions.hpp"
#include "keyboard/keybind.hpp"
#include "service-registry.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

void addCalculatorPasteCopyActions(ActionPanelSectionState *section, AbstractAction *paste,
                                   AbstractAction *copy) {
  paste->addShortcut(Keybind::PasteAction);
  copy->addShortcut(Keybind::CopyAction);

  QString defaultAction;
  if (auto *reg = ServiceRegistry::instance(); reg && reg->rootItemManager()) {
    defaultAction = reg->rootItemManager()
                        ->getProviderPreferenceValues(QStringLiteral("calculator"))
                        .value("defaultAction")
                        .toString();
  }

  if (defaultAction == QLatin1String("copy")) {
    section->addAction(copy);
    section->addAction(paste);
  } else {
    section->addAction(paste);
    section->addAction(copy);
  }
}

void OpenCalculatorHistoryAction::execute(ApplicationContext *ctx) {
  ctx->navigation->activateEntrypoint(EntrypointId{"calculator", "history"});
}
