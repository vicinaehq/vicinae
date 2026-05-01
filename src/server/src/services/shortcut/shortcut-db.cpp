#include <algorithm>
#include <format>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <QDateTime>
#include "shortcut-db.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;

using namespace shortcut;

ShortcutDatabase::ShortcutDatabase(const std::filesystem::path &path) : m_path(path) {
  if (!fs::is_regular_file(m_path)) {
    fs::create_directories(path.parent_path());
    if (const auto result = setShortcuts({}); !result) {
      qCritical() << "Unable to create default shortcuts file at" << m_path.c_str() << result.error().c_str();
    }
  }

  m_shortcuts = loadShortcuts().value_or({});
}

std::expected<std::vector<SerializedShortcut>, std::string> ShortcutDatabase::loadShortcuts() {
  std::vector<SerializedShortcut> shortcuts;

  if (const auto error = glz::read_file_json(shortcuts, m_path.c_str(), m_buf)) {
    return std::unexpected(glz::format_error(error));
  }

  return shortcuts;
}

const std::vector<SerializedShortcut> &ShortcutDatabase::shortcuts() const { return m_shortcuts; }

shortcut::SerializedShortcut *ShortcutDatabase::findById(std::string_view id) {
  if (auto it = std::ranges::find_if(m_shortcuts, [&](auto &&item) { return item.id == id; });
      it != m_shortcuts.end()) {
    return &*it;
  }
  return nullptr;
}

std::expected<SerializedShortcut, std::string> ShortcutDatabase::addShortcut(std::string_view name,
                                                                             std::string_view icon,
                                                                             std::string_view url,
                                                                             std::string_view app) {
  if (m_shortcuts.size() >= MAX_SHORTCUTS) {
    return std::unexpected(std::format("Shortcut limit reached ({})", MAX_SHORTCUTS));
  }

  auto now = static_cast<std::uint64_t>(QDateTime::currentSecsSinceEpoch());

  SerializedShortcut serialized = {
      .id = generatePrefixedId("sct"),
      .name = std::string(name),
      .icon = std::string(icon),
      .url = std::string(url),
      .app = std::string(app),
      .createdAt = now,
      .updatedAt = now,
  };

  m_shortcuts.emplace_back(serialized);

  if (const auto result = setShortcuts(m_shortcuts); !result) {
    m_shortcuts.pop_back();
    return std::unexpected(result.error());
  }

  return serialized;
}

std::expected<void, std::string> ShortcutDatabase::updateShortcut(std::string_view id, std::string_view name,
                                                                  std::string_view icon, std::string_view url,
                                                                  std::string_view app) {
  if (const auto shortcut = findById(id)) {
    shortcut->name = name;
    shortcut->icon = icon;
    shortcut->url = url;
    shortcut->app = app;
    shortcut->updatedAt = static_cast<std::uint64_t>(QDateTime::currentSecsSinceEpoch());

    return setShortcuts(m_shortcuts);
  }

  return std::unexpected("No shortcut with that ID");
}

std::expected<SerializedShortcut, std::string> ShortcutDatabase::removeShortcut(std::string_view id) {
  if (auto it = std::ranges::find_if(m_shortcuts, [&](auto &&item) { return item.id == id; });
      it != m_shortcuts.end()) {
    auto shortcut = *it;
    m_shortcuts.erase(it);

    if (const auto result = setShortcuts(m_shortcuts); !result) { return std::unexpected(result.error()); }

    return shortcut;
  }

  return std::unexpected("No such shortcut");
}

std::expected<void, std::string> ShortcutDatabase::registerVisit(std::string_view id) {
  if (const auto shortcut = findById(id)) {
    shortcut->openCount++;
    shortcut->lastUsedAt = static_cast<std::uint64_t>(QDateTime::currentSecsSinceEpoch());

    return setShortcuts(m_shortcuts);
  }

  return std::unexpected("No shortcut with that ID");
}

void ShortcutDatabase::reload() { m_shortcuts = loadShortcuts().value_or({}); }

std::expected<void, std::string> ShortcutDatabase::setShortcuts(std::span<SerializedShortcut> shortcuts) {
  if (const auto error = glz::write_file_json(shortcuts, m_path.c_str(), m_buf)) {
    return std::unexpected(std::format("Failed to save shortcuts on disk: {}", glz::format_error(error)));
  }

  return {};
}
