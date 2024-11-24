#include "calculator.hpp"

// clang-format off
static std::vector<Unit> units = {
	{ Unit::Type::Milliseconds, Unit::Category::Time, { "ms", "milliseconds" }, "milliseconds", 1 },
	{ Unit::Type::Seconds, Unit::Category::Time, { "second", "seconds", "secs", "sec" }, "seconds", 1000 },
	{ Unit::Type::Minutes, Unit::Category::Time, {  "minute", "minutes", "mins", "min" }, "minutes", 60000 },
	{ Unit::Type::Hours, Unit::Category::Time, { "hour", "hours", "hrs" }, "hours", 3600000 },
	{ Unit::Type::Days, Unit::Category::Time, { "day", "days" }, "days", 86400000 },
	{ Unit::Type::Years, Unit::Category::Time, { "year", "years" }, "years", 31557600000 },

	{ Unit::Type::Bytes, Unit::Category::DigitalStorage, { "byte", "bytes" }, "bytes", 1 },
	{ Unit::Type::Kilobytes, Unit::Category::DigitalStorage, { "kb", "kilobyte", "kilobytes" }, "kilobytes", 1e3 },
	{ Unit::Type::Megabytes, Unit::Category::DigitalStorage, {  "mb", "megabyte", "megabytes" }, "megabytes",  1e6 },
	{ Unit::Type::Gigabytes, Unit::Category::DigitalStorage, { "gb", "gigabyte", "gigabytes" }, "gigabytes", 1e9 },

	{ Unit::Type::Kibibytes, Unit::Category::DigitalStorage, { "kib", "kibibyte", "kibibytes" }, "kibibytes", 1024 },
	{ Unit::Type::Mebibytes, Unit::Category::DigitalStorage, {  "mib", "mibibyte", "mibibytes" }, "mebibytes",  1048576 },
	{ Unit::Type::Gibibytes, Unit::Category::DigitalStorage, { "gib", "gigabyte", "gigabytes" }, "gibibytes", 1073741824 },

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

std::optional<std::reference_wrapper<const Unit>>
findUnitByName(std::string_view name) {
  auto pred = [name](const Unit &a) {
    return std::find(a.names.begin(), a.names.end(), name) != a.names.end();
  };

  if (auto it = std::find_if(units.begin(), units.end(), pred);
      it != units.end())
    return *it;

  return std::nullopt;
}
