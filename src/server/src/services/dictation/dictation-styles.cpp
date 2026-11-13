#include "dictation-styles.hpp"
#include <algorithm>
#include <utility>
#include <QUuid>

DictationStyles::DictationStyles(std::filesystem::path path, QObject *parent)
    : QObject(parent), m_file(std::move(path)) {}

const DictationStyles::Style *DictationStyles::find(std::string_view id) const {
  const auto styles = entries();
  const auto it = std::ranges::find(styles, id, &Style::id);
  return it == styles.end() ? nullptr : &*it;
}

const DictationStyles::Style *DictationStyles::findByName(std::string_view name) const {
  const auto styles = entries();
  const auto it = std::ranges::find(styles, name, &Style::name);
  return it == styles.end() ? nullptr : &*it;
}

const DictationStyles::Style *DictationStyles::owner(std::string_view app) const {
  const auto styles = entries();
  const auto it = std::ranges::find_if(
      styles, [&](const Style &style) { return std::ranges::contains(style.apps, app); });
  return it == styles.end() ? nullptr : &*it;
}

const DictationStyles::Style *DictationStyles::forApp(std::optional<std::string_view> app) const {
  if (app) {
    if (const auto *style = owner(*app)) return style;
  }
  const auto &fallback = m_file.data().defaultStyle;
  return fallback ? find(*fallback) : nullptr;
}

bool DictationStyles::isDefault(std::string_view id) const { return m_file.data().defaultStyle == id; }

std::string DictationStyles::add(Style style) {
  style.id = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
  auto id = style.id;
  m_file.data().styles.emplace_back(std::move(style));
  commit();
  return id;
}

bool DictationStyles::update(Style style) {
  auto &styles = m_file.data().styles;
  const auto it = std::ranges::find(styles, style.id, &Style::id);
  if (it == styles.end()) return false;
  *it = std::move(style);
  return commit();
}

bool DictationStyles::remove(std::string_view id) {
  auto &data = m_file.data();
  if (std::erase_if(data.styles, [&](const Style &style) { return style.id == id; }) == 0) return false;
  if (data.defaultStyle == id) data.defaultStyle.reset();
  return commit();
}

bool DictationStyles::setDefault(std::optional<std::string> id) {
  if (id && !find(*id)) return false;
  m_file.data().defaultStyle = std::move(id);
  return commit();
}

bool DictationStyles::commit() {
  const bool saved = m_file.save();
  emit changed();
  return saved;
}
