#include "group.hpp"

namespace xdgpp {
DesktopEntryGroup::DesktopEntryGroup(const std::string &name) : m_name(name) {}

std::optional<DesktopEntryGroup::LocalizedValue> DesktopEntryGroup::localizedKey(std::string_view name) {
  if (auto it = m_entries.find(std::string(name)); it != m_entries.end()) {
    if (auto &loc = it->second.localized) { return loc.value(); }
  }

  return {};
}

const std::string &DesktopEntryGroup::name() const { return m_name; }

void DesktopEntryGroup::set(const std::string &key, const std::string &value) {
  m_entries[key].value = value;
}

void DesktopEntryGroup::setLocalized(const std::string &key, const std::string &value, const Locale &locale) {
  m_entries[key].localized = LocalizedValue{value, locale};
}

std::optional<std::string> DesktopEntryGroup::rawKey(std::string_view name) const {
  if (auto it = m_entries.find(std::string(name)); it != m_entries.end()) { return it->second.value; }

  return {};
}

std::optional<DesktopEntryValueType> DesktopEntryGroup::key(std::string_view name,
                                                            bool preferLocalized) const {
  if (auto it = m_entries.find(std::string(name)); it != m_entries.end()) {
    if (auto &loc = it->second.localized; loc && preferLocalized) { return ValueType(loc->value); }

    return ValueType(it->second.value);
  }

  return {};
}
}; // namespace xdgpp
