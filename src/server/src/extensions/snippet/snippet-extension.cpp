#include <algorithm>
#include "extensions/snippet/snippet-extension.hpp"
#include "services/snippet/snippet-service.hpp"
#include "service-registry.hpp"

void SnippetExtension::initialized(const QJsonObject &preferences) const {
  preferenceValuesChanged(preferences);
}

void SnippetExtension::preferenceValuesChanged(const QJsonObject &value) const {
  auto *snippet = ServiceRegistry::instance()->snippetService();
  snippet->setEnabled(value.value("enabled").toBool(true));
  snippet->setUndoEnabled(value.value("undo").toBool(true));
  snippet->setPrePasteDelay(
      std::clamp(value.value("prePasteDelay")
                     .toString(QString::number(SnippetService::DEFAULT_PRE_PASTE_DELAY_MS))
                     .toInt(),
                 0, 5000));
  snippet->setKeyDelay(std::clamp(value.value("keyDelay")
                                      .toString(QString::number(SnippetService::DEFAULT_KEY_DELAY_US / 1000))
                                      .toInt(),
                                  0, 50) *
                       1000);

  if (value.contains("layout")) { snippet->setLayout(value.value("layout").toString().toStdString()); }
}

std::vector<Preference> SnippetExtension::preferences() const {
  auto enabled = Preference::makeCheckbox("enabled");
  enabled.setTitle("Expansion");
  enabled.setDescription("Enable automatic snippet expansion when triggers are typed");
  enabled.setDefaultValue(true);

  auto undo = Preference::makeCheckbox("undo");
  undo.setTitle("Undo");
  undo.setDescription("Press backspace immediately after expansion to undo and restore the trigger text");
  undo.setDefaultValue(true);

  auto layout = Preference::makeText("layout");
  layout.setTitle("Keyboard layout");
  layout.setDescription(
      "XKB layout used for trigger detection (e.g. \"us\", \"fr\"). Leave empty for system default.");
  layout.setRequired(false);
  layout.setDefaultValue("");

  auto prePasteDelay = Preference::makeText("prePasteDelay");
  prePasteDelay.setTitle("Pre-paste delay (ms)");
  prePasteDelay.setDescription(
      "Delay between setting clipboard and injecting paste shortcut. Increase if expansions paste empty on "
      "slow compositors.");
  prePasteDelay.setRequired(false);
  prePasteDelay.setDefaultValue(QString::number(SnippetService::DEFAULT_PRE_PASTE_DELAY_MS));

  auto keyDelay = Preference::makeText("keyDelay");
  keyDelay.setTitle("Key injection delay (ms)");
  keyDelay.setDescription("Delay between injected key events. Increase if expansions produce missing or "
                          "garbled characters on slow "
                          "compositors.");
  keyDelay.setRequired(false);
  keyDelay.setDefaultValue(QString::number(SnippetService::DEFAULT_KEY_DELAY_US / 1000));

  return {enabled, undo, layout, prePasteDelay, keyDelay};
}
