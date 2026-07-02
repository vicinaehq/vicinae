#pragma once
#include <cstdint>
#include <span>
#include <string_view>

namespace glyph {

enum class Kind : std::uint8_t {
  Emoji,
  Symbol,
};

enum class Category : std::uint8_t {
  SmileysAndEmotion,
  PeopleAndBody,
  AnimalsAndNature,
  FoodAndDrink,
  TravelAndPlaces,
  Activities,
  Objects,
  Symbols,
  Flags,
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
  std::string_view character;
  std::string_view name;
  std::span<const std::string_view> keywords;
  Kind kind;
  Category category;
  bool skinnable;
};

struct Section {
  Category category;
  Kind kind;
  std::string_view label;
  std::span<const Item> members;
};

std::span<const Item> items();
std::span<const Section> sections();
const Item *lookup(std::string_view character);
std::string_view categoryLabel(Category category);

}; // namespace glyph
