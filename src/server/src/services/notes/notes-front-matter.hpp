#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace Notes {

/** The Markdown following the YAML front matter, or all of it when there is none. */
std::string_view body(std::string_view markdown);

/** Tags of the front matter `tags` key, whether written as a flow list, a block list or plain text. */
std::vector<std::string> tags(std::string_view markdown);

/** Rewrites the `tags` key only: the rest of the front matter is kept byte for byte. */
std::string withTags(std::string_view markdown, std::span<const std::string> tags);

/** Tags never need YAML quoting: letters, digits, `-`, `_` and `/` only. */
bool isValidTag(std::string_view tag);

} // namespace Notes
