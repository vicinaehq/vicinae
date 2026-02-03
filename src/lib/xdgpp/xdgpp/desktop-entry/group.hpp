#pragma once
#include "../locale/locale.hpp"
#include "value.hpp"
#include <string>
#include <unordered_map>

namespace xdgpp {
class DesktopEntryGroup {
  struct LocalizedValue {
    std::string value;
    Locale locale;
  };

  struct Entry {
    std::string value;
    std::optional<LocalizedValue> localized;
  };

  using ValueType = DesktopEntryValueType;

public:
  DesktopEntryGroup(const std::string &name);

  const std::string &name() const;

  void set(const std::string &key, const std::string &value);

  std::unordered_map<std::string, Entry> entries() const { return m_entries; }

  void setLocalized(const std::string &key, const std::string &value, const Locale &locale);

  /**
   * Returns the raw representation of the value associated to the passed key,
   * if any. This is the value without all the specific escape rules applied.
   *
   * Note that leading and trailing spaces are never considered to be
   * part of the value, even in raw form.
   */
  std::optional<std::string> rawKey(std::string_view name) const;

  std::optional<LocalizedValue> localizedKey(std::string_view name);
  std::optional<ValueType> key(std::string_view name, bool preferLocalized = true) const;

private:
  std::string m_name;
  std::unordered_map<std::string, Entry> m_entries;
};

} // namespace xdgpp
