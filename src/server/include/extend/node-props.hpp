#pragma once
#include <glaze/json/generic.hpp>
#include <optional>
#include <string>
#include <string_view>

namespace node_props {

inline const glz::generic *get(const glz::generic::object_t &props, std::string_view key) {
  auto it = props.find(key);
  return it != props.end() ? &it->second : nullptr;
}

inline bool has(const glz::generic::object_t &props, std::string_view key) {
  return props.find(key) != props.end();
}

inline std::string getStringOr(const glz::generic::object_t &props, std::string_view key,
                               std::string_view def = "") {
  auto *v = get(props, key);
  if (v && v->is_string()) return v->get_string();
  return std::string{def};
}

inline std::optional<std::string_view> getString(const glz::generic::object_t &props, std::string_view key) {
  auto *v = get(props, key);
  if (v && v->is_string()) return std::string_view{v->get_string()};
  return std::nullopt;
}

inline bool getBool(const glz::generic::object_t &props, std::string_view key, bool def = false) {
  auto *v = get(props, key);
  if (v && v->is_boolean()) return v->get_boolean();
  return def;
}

inline double getDouble(const glz::generic::object_t &props, std::string_view key, double def = 0.0) {
  auto *v = get(props, key);
  if (v && v->is_number()) return v->get_number();
  return def;
}

inline int getInt(const glz::generic::object_t &props, std::string_view key, int def = 0) {
  auto *v = get(props, key);
  if (v && v->is_number()) return static_cast<int>(v->get_number());
  return def;
}

inline const glz::generic::array_t *getArray(const glz::generic::object_t &props, std::string_view key) {
  auto *v = get(props, key);
  if (v && v->is_array()) return &v->get_array();
  return nullptr;
}

inline const glz::generic::object_t *getObject(const glz::generic::object_t &props, std::string_view key) {
  auto *v = get(props, key);
  if (v && v->is_object()) return &v->get_object();
  return nullptr;
}

} // namespace node_props
