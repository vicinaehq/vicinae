#pragma once
#include <map>
#include <string>
#include <string_view>
#include <glaze/json/generic.hpp>
#include <QCoreApplication>
#include "command/preference-schema.hpp"

namespace AI {

constexpr auto EXTENSION_ID = std::string_view("ai");
constexpr auto PROVIDERS_PREFERENCE = std::string_view("providers");

struct ProviderInstance {
  std::string type;
  glz::generic::object_t fields;
};

} // namespace AI

struct AiPreferences {
  std::map<std::string, AI::ProviderInstance> providers;
  bool enableTools = true;
};

template <> struct PreferenceSchema<AiPreferences> {
  PreferenceMeta providers{
      .title = tr("Providers"),
      .description = tr("Local and cloud AI providers. Their models power dictation and Quick AI."),
      .component = QStringLiteral("AISettingsPage"),
      .kind = PreferenceMeta::Kind::Custom,
  };
  PreferenceMeta enableTools{
      .title = tr("Enable tools"),
      .description = tr("Allow AI models to use tools from enabled extensions."),
  };
  Q_DECLARE_TR_FUNCTIONS(AiPreferences)
};
