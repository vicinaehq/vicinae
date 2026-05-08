#pragma once
#include <string_view>
#include <array>
#include <unordered_map>
#include <span>

struct EmojiData {
	std::string_view emoji;
	std::string_view name;
	std::string_view group;
	std::span<const std::string_view> keywords;
	bool skinToneSupport = false;
};

class StaticEmojiDatabase {
	public:
		StaticEmojiDatabase() = delete;
		static const std::array<EmojiData, 1906>& orderedList();
		static const std::unordered_map<std::string_view, const EmojiData*>& mapping();
		static const std::array<std::string_view, 9>& groups();
};
