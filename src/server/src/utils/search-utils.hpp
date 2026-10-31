#pragma once
#include <string>
#include <string_view>

namespace search_utils {

/**
 * Detects whether a query string has mathematical, arithmetic,
 * or conversion intent (e.g. '12*4', '100 usd to eur', 'sqrt(16)').
 */
bool hasMathIntent(std::string_view query);

/**
 * Extracts acronym initials from a title or phrase (e.g. 'Google Chrome' -> 'gc',
 * 'Visual Studio Code' -> 'vsc', 'VirtualBox' -> 'vb').
 */
std::string extractAcronym(std::string_view text);

/**
 * Calculates acronym / initialism matching bonus for a given search query against a title.
 * Returns a score boost (e.g. 35.0 for exact acronym match, 20.0 for prefix match, 0.0 otherwise).
 */
double calculateAcronymBonus(std::string_view query, std::string_view title);

} // namespace search_utils
