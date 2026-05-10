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

  return {enabled, undo, layout};
}
