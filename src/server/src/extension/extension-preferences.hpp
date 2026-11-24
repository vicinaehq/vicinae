#pragma once
#include <span>
#include "command/preference.hpp"

namespace preferences {

inline PreferenceValues resolveApplications(PreferenceValues values, std::span<const Preference> definitions,
                                            const auto &resolve) {
  for (const auto &pref : definitions) {
    const auto data = pref.data();
    const auto *picker = std::get_if<Preference::AppPickerData>(&data);
    if (!picker) continue;
    const auto it = values.find(pref.name().toStdString());
    if (it == values.end()) continue;

    if (picker->multiple) {
      glz::generic::array_t apps;
      if (it->second.is_array()) {
        const auto &ids = it->second.get_array();
        apps.reserve(ids.size());
        for (const auto &id : ids) {
          if (!id.is_string()) continue;
          if (auto app = resolve(id.get_string()); !app.is_null()) apps.emplace_back(std::move(app));
        }
      }
      it->second = std::move(apps);
    } else {
      it->second = it->second.is_string() ? resolve(it->second.get_string()) : glz::generic{};
    }
  }
  return values;
}

} // namespace preferences
