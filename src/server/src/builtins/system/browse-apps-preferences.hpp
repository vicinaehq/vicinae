#pragma once
#include <QCoreApplication>
#include "command/preference-schema.hpp"

struct BrowseAppsPreferences {
  bool sortAlphabetically = true;
  bool showHidden = false;
};

template <> struct PreferenceSchema<BrowseAppsPreferences> {
  PreferenceMeta sortAlphabetically{.label = tr("Sort alphabetically")};
  PreferenceMeta showHidden{.label = tr("Show hidden apps")};
  Q_DECLARE_TR_FUNCTIONS(BrowseAppsPreferences)
};
