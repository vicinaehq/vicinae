#include "services/files-service/file-indexer/regex-utils.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("extractStaticCharsFromRegex - basic patterns", "[file-indexer]") {
  SECTION(".*config.*db^") { REQUIRE(extractStaticCharsFromRegex(".*config.*db^") == "config db"); }
  SECTION("^[ab]cd") { REQUIRE(extractStaticCharsFromRegex("^[ab]cd") == "cd"); }
  SECTION("^ab[cd](test)k") { REQUIRE(extractStaticCharsFromRegex("^ab[cd](test)k") == "^ab test k"); }
}

TEST_CASE("extractStaticCharsFromRegex - special regex characters", "[file-indexer]") {
  SECTION("handles dot wildcard") { REQUIRE(extractStaticCharsFromRegex("a.b") == "a b"); }
  SECTION("handles asterisk") { REQUIRE(extractStaticCharsFromRegex("a*b") == "a b"); }
  SECTION("handles plus") { REQUIRE(extractStaticCharsFromRegex("a+b") == "a b"); }
  SECTION("handles question mark") { REQUIRE(extractStaticCharsFromRegex("a?b") == "b"); }
  SECTION("handles pipe") { REQUIRE(extractStaticCharsFromRegex("a|b") == ""); }
  SECTION("handles dollar sign") { REQUIRE(extractStaticCharsFromRegex("abc$") == "abc"); }
}

TEST_CASE("extractStaticCharsFromRegex - keeps only alphanumercial, space, tab, newline", "[file-indexer]") {
  SECTION("strips dots") { REQUIRE(extractStaticCharsFromRegex("abc\\.") == "abc"); }
  SECTION("strips dots") { REQUIRE(extractStaticCharsFromRegex("\\\\-$@#abc\\.") == "abc"); }
  SECTION("strips dots") { REQUIRE(extractStaticCharsFromRegex("abc-def") == "abc def"); }
}

TEST_CASE("extractStaticCharsFromRegex - brackets", "[file-indexer]") {
  SECTION("ignores content in square brackets") { REQUIRE(extractStaticCharsFromRegex("[abc]def") == "def"); }

  SECTION("keeps content in parentheses (groups)") {
    REQUIRE(extractStaticCharsFromRegex("(abc)def") == "abcdef");
  }
}

TEST_CASE("extractStaticCharsFromRegex - caret handling", "[file-indexer]") {
  SECTION("keeps caret followed by regular character") {
    REQUIRE(extractStaticCharsFromRegex("^abc") == "^abc");
  }

  SECTION("removes caret not followed by character") { REQUIRE(extractStaticCharsFromRegex("^[abc]") == ""); }

  SECTION("removes caret followed by special char") {
    REQUIRE(extractStaticCharsFromRegex("^.abc") == "abc");
  }

  SECTION("caret in middle without following char") { REQUIRE(extractStaticCharsFromRegex("abc^") == "abc"); }
}

TEST_CASE("extractStaticCharsFromRegex - escaping", "[file-indexer]") {
  SECTION("escaped special characters become static") {
    REQUIRE(extractStaticCharsFromRegex("a\\.b") == "a b");
  }

  SECTION("escaped bracket") { REQUIRE(extractStaticCharsFromRegex("a\\[b") == "a b"); }
  SECTION("escaped backslash") { REQUIRE(extractStaticCharsFromRegex("a\\\\b") == "a b"); }
  SECTION("caret with escaped character") { REQUIRE(extractStaticCharsFromRegex("^\\[test") == "test"); }
}

TEST_CASE("extractStaticCharsFromRegex - empty and edge cases", "[file-indexer]") {
  SECTION("empty string") { REQUIRE(extractStaticCharsFromRegex("") == ""); }
  SECTION("only special characters") { REQUIRE(extractStaticCharsFromRegex(".*+?|$") == ""); }
  SECTION("only brackets") { REQUIRE(extractStaticCharsFromRegex("[abc](def)") == "def"); }
  SECTION("single character") { REQUIRE(extractStaticCharsFromRegex("a") == "a"); }
}

TEST_CASE("extractStaticCharsFromRegex - quantifiers", "[file-indexer]") {
  SECTION("exactly n - {3}") { REQUIRE(extractStaticCharsFromRegex("a{3}b") == "a b"); }
  SECTION("n or more - {3,}") { REQUIRE(extractStaticCharsFromRegex("a{3,}b") == "a b"); }
  SECTION("between m and n - {3,5}") { REQUIRE(extractStaticCharsFromRegex("a{3,5}b") == "a b"); }

  SECTION("lazy quantifiers - ?? *? +?") {
    REQUIRE(extractStaticCharsFromRegex("a??b") == "b");
    REQUIRE(extractStaticCharsFromRegex("a*?b") == "a b");
    REQUIRE(extractStaticCharsFromRegex("a+?b") == "a b");
  }

  SECTION("multiple wildcards") { REQUIRE(extractStaticCharsFromRegex("a.*b.*c") == "a b c"); }
  SECTION("combined quantifiers") { REQUIRE(extractStaticCharsFromRegex("a+b*c?d") == "a b d"); }
}

TEST_CASE("extractStaticCharsFromRegex - word boundaries", "[file-indexer]") {
  SECTION("word boundary \\b") { REQUIRE(extractStaticCharsFromRegex("\\btest\\b") == "test"); }
  SECTION("not word boundary \\B") { REQUIRE(extractStaticCharsFromRegex("\\Btest\\B") == "test"); }
  SECTION("start of word \\<") { REQUIRE(extractStaticCharsFromRegex("\\<test") == "test"); }
  SECTION("end of word \\>") { REQUIRE(extractStaticCharsFromRegex("test\\>") == "test"); }
}

TEST_CASE("extractStaticCharsFromRegex - character classes", "[file-indexer]") {
  SECTION("digit \\d") { REQUIRE(extractStaticCharsFromRegex("\\d") == ""); }
  SECTION("not digit \\D") { REQUIRE(extractStaticCharsFromRegex("\\D") == ""); }
  SECTION("whitespace \\s") { REQUIRE(extractStaticCharsFromRegex("\\s") == ""); }
  SECTION("not whitespace \\S") { REQUIRE(extractStaticCharsFromRegex("\\S") == ""); }
  SECTION("word character \\w") { REQUIRE(extractStaticCharsFromRegex("\\w") == ""); }
  SECTION("not word character \\W") { REQUIRE(extractStaticCharsFromRegex("\\W") == ""); }
  SECTION("hex digit \\x") { REQUIRE(extractStaticCharsFromRegex("\\x41") == "A"); }
}

TEST_CASE("extractStaticCharsFromRegex - complex patterns", "[file-indexer]") {
  SECTION("multiple groups") { REQUIRE(extractStaticCharsFromRegex("(abc)(def)(ghi)") == "abcdefghi"); }
  SECTION("nested groups") { REQUIRE(extractStaticCharsFromRegex("((ab)c)def") == "abcdef"); }
  SECTION("alternation in group") { REQUIRE(extractStaticCharsFromRegex("(a|b)cd") == "cd"); }

  SECTION("mixed brackets and groups") {
    REQUIRE(extractStaticCharsFromRegex("[abc](def)[ghi]jkl") == "def jkl");
  }

  SECTION("quantifiers with groups") { REQUIRE(extractStaticCharsFromRegex("(abc)+def") == "abc def"); }
}

TEST_CASE("extractStaticCharsFromRegex - anchors", "[file-indexer]") {
  SECTION("\\A start of string") { REQUIRE(extractStaticCharsFromRegex("\\Atest") == "test"); }
  SECTION("\\Z end of string") { REQUIRE(extractStaticCharsFromRegex("test\\Z") == "test"); }
  SECTION("combined anchors") { REQUIRE(extractStaticCharsFromRegex("^test$") == "^test"); }
}

TEST_CASE("extractStaticCharsFromRegex - special escapes", "[file-indexer]") {
  SECTION("tab \\t") { REQUIRE(extractStaticCharsFromRegex("a\\tb") == "a\tb"); }
  SECTION("newline \\n") { REQUIRE(extractStaticCharsFromRegex("a\\nb") == "a\nb"); }
  SECTION("carriage return \\r") { REQUIRE(extractStaticCharsFromRegex("a\\rb") == "a\rb"); }
  SECTION("vertical tab \\v") { REQUIRE(extractStaticCharsFromRegex("a\\vb") == "a\vb"); }
  SECTION("form feed \\f") { REQUIRE(extractStaticCharsFromRegex("a\\fb") == "a\fb"); }
}

TEST_CASE("extractStaticCharsFromRegex - minWordLength", "[file-indexer]") {
  SECTION("filters single character words") {
    REQUIRE(extractStaticCharsFromRegex("a b c", 2) == "");
    REQUIRE(extractStaticCharsFromRegex("ab cd ef", 2) == "ab cd ef");
    REQUIRE(extractStaticCharsFromRegex("a bc d", 2) == "bc");
  }

  SECTION("filters with minimum length 3") {
    REQUIRE(extractStaticCharsFromRegex("a bc def", 3) == "def");
    REQUIRE(extractStaticCharsFromRegex("foo bar ab", 3) == "foo bar");
    REQUIRE(extractStaticCharsFromRegex("test a b", 3) == "test");
  }

  SECTION("keeps all words when min is 0") {
    REQUIRE(extractStaticCharsFromRegex("a b c", 0) == "a b c");
    REQUIRE(extractStaticCharsFromRegex("test a bc", 0) == "test a bc");
  }

  SECTION("regex patterns with minWordLength") {
    REQUIRE(extractStaticCharsFromRegex(".*config.*db^", 3) == "config");
    REQUIRE(extractStaticCharsFromRegex("^ab[cd](test)k", 3) == "test");
    REQUIRE(extractStaticCharsFromRegex("a.*b.*c", 2) == "");
  }

  SECTION("empty result when all words too short") {
    REQUIRE(extractStaticCharsFromRegex("a b c d e", 3) == "");
    REQUIRE(extractStaticCharsFromRegex("ab cd", 3) == "");
  }

  SECTION("preserves word boundaries") {
    REQUIRE(extractStaticCharsFromRegex("test a b config", 4) == "test config");
    REQUIRE(extractStaticCharsFromRegex("a very long test", 4) == "very long test");
  }
}
