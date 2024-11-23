#include "calculator.hpp"

// clang-format off
static std::vector<Unit> units = {
	{
		.type = Unit::Type::Seconds,
		.category = Unit::Category::Time,
		.names = {  "second", "seconds", "secs" },
		.displayName = "seconds",
		.conversionFactor = 1,
	},
	{
		.type = Unit::Type::Minutes,
		.category = Unit::Category::Time,
		.names = {  "minute", "minutes", "mins", "min" },
		.displayName = "minutes",
		.conversionFactor = 60,
	},
	{
		.type = Unit::Type::Hours,
		.category = Unit::Category::Time,
		.names = {  "hour", "hours", "hrs" },
		.displayName = "hours",
		.conversionFactor = 3600,
	},
	{
		.type = Unit::Type::Days,
		.category = Unit::Category::Time,
		.names = {  "day", "days" },
		.displayName = "days",
		.conversionFactor = 86400,
	},
	{
		.type = Unit::Type::Years,
		.category = Unit::Category::Time,
		.names = {  "year", "years" },
		.displayName = "years",
		.conversionFactor = 31557600
	},
	{
		.type = Unit::Type::Grams,
		.category = Unit::Category::Weight,
		.names = {  "g", "gram", "grams" },
		.displayName = "grams",
		.conversionFactor = 1,
	},
	{
		.type = Unit::Type::Pounds,
		.category = Unit::Category::Weight,
		.names = {  "pound", "pounds" },
		.displayName = "pounds",
		.conversionFactor = 0.00220462262185
	},

	 // Weight Units (Metric)
    { Unit::Type::Grams, Unit::Category::Weight, { "g", "gram", "grams" }, "grams", 1 },
    { Unit::Type::Kilograms, Unit::Category::Weight, { "kg", "kilogram", "kilograms" }, "kilograms", 1000 },

    // Weight Units (US Customary)
    { Unit::Type::Ounces, Unit::Category::Weight, { "oz", "ounce", "ounces" }, "ounces", 28.3495 }, // grams per ounce
    { Unit::Type::Pounds, Unit::Category::Weight, { "lb", "pound", "pounds" }, "pounds", 453.592 }, // grams per pound
    { Unit::Type::TonsUS, Unit::Category::Weight, { "ton", "tons", "us ton" }, "US tons", 907184.74 }, // grams per ton

    // Length Units (Metric)
    { Unit::Type::Millimeters, Unit::Category::Length, { "mm", "millimeter", "millimeters" }, "millimeters", 1 },
    { Unit::Type::Centimeters, Unit::Category::Length, { "cm", "centimeter", "centimeters" }, "centimeters", 10 },
    { Unit::Type::Meters, Unit::Category::Length, { "m", "meter", "meters" }, "meters", 1000 },
    { Unit::Type::Kilometers, Unit::Category::Length, { "km", "kilometer", "kilometers" }, "kilometers", 1e6 },

    // Length Units (US Customary)
    { Unit::Type::Inches, Unit::Category::Length, { "inch", "inches" }, "inches", 25.4 }, // millimeters per inch
    { Unit::Type::Feet, Unit::Category::Length, { "ft", "foot", "feet" }, "feet", 304.8 }, // millimeters per foot
    { Unit::Type::Yards, Unit::Category::Length, { "yd", "yard", "yards" }, "yards", 914.4 }, // millimeters per yard
    { Unit::Type::Miles, Unit::Category::Length, { "mi", "mile", "miles" }, "miles", 1.60934e6 } // millimeters per mile
};
// clang-format on

static std::map<Unit::Type, Unit::Type> defaultUnits{
    {Unit::Type::Grams, Unit::Type::Ounces}};

std::optional<Unit> findUnitByName(std::string_view s) {
  for (const auto &unit : units) {
    for (const auto &name : unit.names) {
      if (strncasecmp(name.data(), s.data(), name.size()) == 0)
        return unit;
    }
  }

  return std::nullopt;
}
