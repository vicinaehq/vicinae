#pragma once
#include <cstdint>
#include <span>
#include <string_view>

namespace glyph {

// What an item is, for kind-specific actions. Symbols and emoji are otherwise
// one unified data source.
enum class Kind : std::uint8_t {
  Emoji,
  Symbol,
};

// Unified category space spanning emoji groups and symbol categories, in
// display order (emoji groups first). This is the filter-dropdown's value set.
// Keep in sync with CATEGORIES in src/categories.ts.
enum class Category : std::uint8_t {
  // Emoji groups (canonical emoji-test order).
  SmileysAndEmotion,
  PeopleAndBody,
  AnimalsAndNature,
  FoodAndDrink,
  TravelAndPlaces,
  Activities,
  Objects,
  Symbols,
  Flags,
  // Symbol categories.
  Math,
  Arrows,
  Currency,
  Punctuation,
  Shapes,
  MiscSymbols,
  Greek,
  NumberForms,
  FancyLetters,
};

struct Item {
  // The glyph (e.g. "😀" or "∮"). Identity key for joining runtime metadata.
  std::string_view character;
  std::string_view name;
  std::span<const std::string_view> keywords;
  Kind kind;
  Category category;
  // True for emoji that accept skin-tone modifiers.
  bool skinnable;
};

// A run of items sharing one category — a contiguous view into items().
// Drives both the filter dropdown and the grid sections.
struct Section {
  Category category;
  Kind kind;
  std::string_view label;
  std::span<const Item> members;
};

// Every item (emoji + symbols) in category order; emoji groups first.
std::span<const Item> items();

// One section per category, in category order. members are contiguous slices
// of items(), so the root grid and dropdown need no runtime bucketing.
std::span<const Section> sections();

// Look up any item by its glyph; nullptr if absent. O(log n).
const Item *lookup(std::string_view character);

// Display label for a category (for the filter dropdown).
std::string_view categoryLabel(Category category);

}; // namespace glyph
