#pragma once
#include <string_view>

namespace search_utils {

/**
 * Detects whether a query string has mathematical, arithmetic,
 * or conversion intent (e.g. '12*4', '100 usd to eur', 'sqrt(16)').
 */
bool hasMathIntent(std::string_view query);

} // namespace search_utils
