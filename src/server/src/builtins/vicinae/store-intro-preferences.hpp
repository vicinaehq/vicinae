#pragma once
#include <QCoreApplication>
#include "command/preference-schema.hpp"

struct StoreIntroPreferences {
  bool alwaysShowIntro = false;
};

template <> struct PreferenceSchema<StoreIntroPreferences> {
  PreferenceMeta alwaysShowIntro{.label = tr("Always show intro")};
  Q_DECLARE_TR_FUNCTIONS(StoreIntroPreferences)
};
