#include <ranges>
#include <string>
#include <string_view>
#include <catch2/catch_test_macros.hpp>
#include "../src/root-search/apps/app-pinyin-keywords.hpp"
#include "fuzzy/fuzzy-searchable.hpp"

TEST_CASE("Application pinyin keywords match full spellings, initials and mixed queries", "[app-pinyin]") {
  const auto matches = [](QStringView name, std::string_view query) {
    std::vector<QString> keywords;
    appendAppPinyinKeywords(name, keywords);

    std::vector<std::string> utf8;
    utf8.reserve(keywords.size());
    for (const auto &word : keywords)
      utf8.emplace_back(word.toStdString());

    using Field = fzf::WeightedString;
    const auto fields = utf8 | std::views::transform([](const auto &word) { return Field{word, 0.6f}; });
    return fzf::threadLocalMatcher().score_query(fields, fuzzy::Query{query}).quality >= fuzzy::MIN_QUALITY;
  };

  CHECK(matches(u"微信", "weixin"));
  CHECK(matches(u"微信", "wx"));
  CHECK(matches(u"微信", "weix"));
  CHECK(matches(u"微信", "wxin"));
  CHECK(matches(u"网易云音乐", "wangyiyunyinyue"));
  CHECK(matches(u"网易云音乐", "wyyyy"));
  CHECK(matches(u"网易云音乐", "wyyunyy"));
  CHECK(matches(u"QQ音乐", "qqyy"));
  CHECK(matches(u"QQ 音乐", "qq yy"));
  CHECK(matches(u"QQ 音乐", "yy"));
}

TEST_CASE("Application pinyin keywords leave Latin names and existing keywords intact", "[app-pinyin]") {
  const auto keyword = QStringLiteral("Browser");
  std::vector<QString> keywords{keyword};
  appendAppPinyinKeywords(u"Firefox", keywords);
  CHECK(keywords == std::vector{keyword});

  appendAppPinyinKeywords(u"微信", keywords);
  REQUIRE_FALSE(keywords.empty());
  CHECK(keywords.front() == keyword);
}
