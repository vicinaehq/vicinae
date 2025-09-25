#pragma once
#include <unordered_map>
#include <vector>
#include <string_view>
#include <filesystem>

namespace xdgpp {

class MimeAppsList {
public:
  using AppList = std::vector<std::string>;
  using Mime = std::string;
  using EntryMap = std::unordered_map<Mime, AppList>;

  static MimeAppsList fromFile(const std::filesystem::path &data);
  static MimeAppsList fromData(std::string_view data);

  std::vector<std::string> addedAssociations(std::string_view mime) const;
  std::vector<std::string> removedAssociations(std::string_view mime) const;
  std::vector<std::string> defaultAssociations(std::string_view mime) const;
  const EntryMap &addedAssociations() const;
  const EntryMap &removedAssociations() const;
  const EntryMap &defaultApplications() const;

private:
  MimeAppsList(const std::filesystem::path &path);
  MimeAppsList(std::string_view data);

  EntryMap m_added;
  EntryMap m_removed;
  EntryMap m_default;
};
}; // namespace xdgpp
