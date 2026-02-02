#include "mime-apps-list.hpp"
#include "../desktop-entry/reader.hpp"
#include "../utils/utils.hpp"

namespace fs = std::filesystem;

namespace xdgpp {
MimeAppsList MimeAppsList::fromFile(const fs::path &path) { return fromData(slurp(path)); }
MimeAppsList MimeAppsList::fromData(std::string_view data) { return data; }

std::vector<std::string> MimeAppsList::addedAssociations(std::string_view mime) const {
  if (auto it = m_added.find(std::string(mime)); it != m_added.end()) return it->second;
  return {};
}

std::vector<std::string> MimeAppsList::removedAssociations(std::string_view mime) const {
  if (auto it = m_removed.find(std::string(mime)); it != m_removed.end()) return it->second;
  return {};
}

std::vector<std::string> MimeAppsList::defaultAssociations(std::string_view mime) const {
  if (auto it = m_default.find(std::string(mime)); it != m_default.end()) return it->second;
  return {};
}

const MimeAppsList::EntryMap &MimeAppsList::addedAssociations() const { return m_added; }
const MimeAppsList::EntryMap &MimeAppsList::removedAssociations() const { return m_removed; }
const MimeAppsList::EntryMap &MimeAppsList::defaultApplications() const { return m_default; }

MimeAppsList::MimeAppsList(std::string_view data) {
  DesktopEntryReader reader(data);

  if (auto added = reader.group("Added Associations")) {
    for (const auto &[k, entry] : added->entries()) {
      m_added[k] = DesktopEntryValueType(entry.value).asStringList();
    }
  }

  if (auto removed = reader.group("Removed Associations")) {
    for (const auto &[k, entry] : removed->entries()) {
      m_removed[k] = DesktopEntryValueType(entry.value).asStringList();
    }
  }

  if (auto dflt = reader.group("Default Applications")) {
    for (const auto &[k, entry] : dflt->entries()) {
      m_default[k] = DesktopEntryValueType(entry.value).asStringList();
    }
  }
}

} // namespace xdgpp
