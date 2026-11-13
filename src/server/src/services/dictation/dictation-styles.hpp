#pragma once
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include <QObject>
#include "utils/json-file.hpp"

/**
 * How dictated text gets rewritten, kept in one JSON file. An app belongs to at most one style, and the
 * default style covers every app without its own.
 */
class DictationStyles : public QObject {
  Q_OBJECT

signals:
  void changed();

public:
  struct Style {
    std::string id;
    std::string name;
    std::string instructions;
    std::string model;
    std::vector<std::string> apps;
  };

  explicit DictationStyles(std::filesystem::path path, QObject *parent = nullptr);

  std::span<const Style> entries() const { return m_file.data().styles; }
  const Style *find(std::string_view id) const;
  const Style *findByName(std::string_view name) const;
  const Style *owner(std::string_view app) const;
  const Style *forApp(std::optional<std::string_view> app) const;
  bool isDefault(std::string_view id) const;

  /**
   * Returns the id of the new style.
   */
  std::string add(Style style);
  bool update(Style style);
  bool remove(std::string_view id);
  bool setDefault(std::optional<std::string> id);

private:
  struct Data {
    std::vector<Style> styles;
    std::optional<std::string> defaultStyle;
  };

  bool commit();

  JsonFile<Data> m_file;
};
