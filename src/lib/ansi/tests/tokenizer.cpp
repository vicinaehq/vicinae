#include <catch2/catch_test_macros.hpp>
#include "ansi/ansi.hpp"

using namespace ansi;

TEST_CASE("plain text passes through unchanged") {
  Tokenizer tok("hello world");
  auto seg = tok.next();
  REQUIRE(seg);
  REQUIRE(seg->text == "hello world");
  REQUIRE(seg->style.is_default());
  REQUIRE_FALSE(tok.next());
}

TEST_CASE("empty input yields nothing") {
  Tokenizer tok("");
  REQUIRE_FALSE(tok.next());
}

TEST_CASE("standard foreground colors") {
  // red text
  Tokenizer tok("\x1b[31mhello");
  auto seg = tok.next();
  REQUIRE(seg);
  REQUIRE(seg->text == "hello");
  REQUIRE(seg->style.fg == Color::standard(1));
  REQUIRE_FALSE(seg->style.bg);
}

TEST_CASE("standard background colors") {
  Tokenizer tok("\x1b[42mhello");
  auto seg = tok.next();
  REQUIRE(seg);
  REQUIRE(seg->style.bg == Color::standard(2));
}

TEST_CASE("bright foreground and background") {
  Tokenizer tok("\x1b[91;104mtext");
  auto seg = tok.next();
  REQUIRE(seg);
  REQUIRE(seg->style.fg == Color::bright(1));
  REQUIRE(seg->style.bg == Color::bright(4));
}

TEST_CASE("256-color palette") {
  Tokenizer tok("\x1b[38;5;208mtext");
  auto seg = tok.next();
  REQUIRE(seg);
  REQUIRE(seg->style.fg == Color::palette(208));
}

TEST_CASE("truecolor") {
  Tokenizer tok("\x1b[38;2;255;128;0mtext");
  auto seg = tok.next();
  REQUIRE(seg);
  REQUIRE(seg->style.fg == Color::rgb(255, 128, 0));
}

TEST_CASE("256-color background") {
  Tokenizer tok("\x1b[48;5;120mtext");
  auto seg = tok.next();
  REQUIRE(seg);
  REQUIRE(seg->style.bg == Color::palette(120));
}

TEST_CASE("truecolor background") {
  Tokenizer tok("\x1b[48;2;10;20;30mtext");
  auto seg = tok.next();
  REQUIRE(seg);
  REQUIRE(seg->style.bg == Color::rgb(10, 20, 30));
}

TEST_CASE("text attributes") {
  SECTION("bold") {
    Tokenizer tok("\x1b[1mtext");
    REQUIRE(tok.next()->style.bold);
  }
  SECTION("dim") {
    Tokenizer tok("\x1b[2mtext");
    REQUIRE(tok.next()->style.dim);
  }
  SECTION("italic") {
    Tokenizer tok("\x1b[3mtext");
    REQUIRE(tok.next()->style.italic);
  }
  SECTION("underline") {
    Tokenizer tok("\x1b[4mtext");
    REQUIRE(tok.next()->style.underline);
  }
  SECTION("strikethrough") {
    Tokenizer tok("\x1b[9mtext");
    REQUIRE(tok.next()->style.strikethrough);
  }
  SECTION("inverse") {
    Tokenizer tok("\x1b[7mtext");
    REQUIRE(tok.next()->style.inverse);
  }
  SECTION("hidden") {
    Tokenizer tok("\x1b[8mtext");
    REQUIRE(tok.next()->style.hidden);
  }
  SECTION("overline") {
    Tokenizer tok("\x1b[53mtext");
    REQUIRE(tok.next()->style.overline);
  }
}

TEST_CASE("combined attributes in one sequence") {
  Tokenizer tok("\x1b[1;3;31mtext");
  auto seg = tok.next();
  REQUIRE(seg);
  REQUIRE(seg->style.bold);
  REQUIRE(seg->style.italic);
  REQUIRE(seg->style.fg == Color::standard(1));
}

TEST_CASE("reset clears all attributes") {
  Tokenizer tok("\x1b[1;31mhello\x1b[0mworld");

  auto seg1 = tok.next();
  REQUIRE(seg1);
  REQUIRE(seg1->text == "hello");
  REQUIRE(seg1->style.bold);
  REQUIRE(seg1->style.fg == Color::standard(1));

  auto seg2 = tok.next();
  REQUIRE(seg2);
  REQUIRE(seg2->text == "world");
  REQUIRE(seg2->style.is_default());
}

TEST_CASE("ESC[m is equivalent to ESC[0m") {
  Tokenizer tok("\x1b[31mhello\x1b[mworld");

  tok.next(); // skip red segment
  auto seg = tok.next();
  REQUIRE(seg);
  REQUIRE(seg->style.is_default());
}

TEST_CASE("individual attribute resets") {
  SECTION("22 resets bold and dim") {
    Tokenizer tok("\x1b[1;2mtext\x1b[22mafter");
    tok.next();
    auto seg = tok.next();
    REQUIRE_FALSE(seg->style.bold);
    REQUIRE_FALSE(seg->style.dim);
  }
  SECTION("39 resets foreground") {
    Tokenizer tok("\x1b[31mtext\x1b[39mafter");
    tok.next();
    auto seg = tok.next();
    REQUIRE_FALSE(seg->style.fg);
  }
  SECTION("49 resets background") {
    Tokenizer tok("\x1b[42mtext\x1b[49mafter");
    tok.next();
    auto seg = tok.next();
    REQUIRE_FALSE(seg->style.bg);
  }
}

TEST_CASE("style accumulates across sequences") {
  Tokenizer tok("\x1b[1m\x1b[31mtext");
  auto seg = tok.next();
  REQUIRE(seg);
  REQUIRE(seg->style.bold);
  REQUIRE(seg->style.fg == Color::standard(1));
}

TEST_CASE("non-SGR CSI sequences are skipped") {
  // ESC[2J is "erase display", should be ignored
  Tokenizer tok("\x1b[2Jhello");
  auto seg = tok.next();
  REQUIRE(seg);
  REQUIRE(seg->text == "hello");
  REQUIRE(seg->style.is_default());
}

TEST_CASE("private-mode CSI sequences are skipped") {
  // ESC[?25h is "show cursor"
  Tokenizer tok("\x1b[?25hhello");
  auto seg = tok.next();
  REQUIRE(seg);
  REQUIRE(seg->text == "hello");
  REQUIRE(seg->style.is_default());
}

TEST_CASE("malformed escape sequences") {
  SECTION("lone ESC at end of input") {
    Tokenizer tok("hello\x1b");
    auto seg = tok.next();
    REQUIRE(seg);
    REQUIRE(seg->text == "hello");
    REQUIRE_FALSE(tok.next());
  }
  SECTION("ESC followed by non-bracket") {
    Tokenizer tok("\x1b)hello");
    auto seg = tok.next();
    REQUIRE(seg);
    REQUIRE(seg->text == "hello");
  }
  SECTION("truncated CSI sequence") {
    Tokenizer tok("hello\x1b[31");
    auto seg = tok.next();
    REQUIRE(seg);
    REQUIRE(seg->text == "hello");
    REQUIRE_FALSE(tok.next());
  }
}

TEST_CASE("input with only escape sequences yields nothing") {
  Tokenizer tok("\x1b[31m\x1b[0m");
  REQUIRE_FALSE(tok.next());
}

TEST_CASE("strip removes all escapes") {
  REQUIRE(strip("\x1b[1;31mhello \x1b[0mworld") == "hello world");
  REQUIRE(strip("plain text") == "plain text");
  REQUIRE(strip("") == "");
  REQUIRE(strip("\x1b[38;2;255;0;0mtruecolor\x1b[0m") == "truecolor");
}

TEST_CASE("extended color with insufficient params is ignored") {
  SECTION("38;5 with no index") {
    Tokenizer tok("\x1b[38;5mtext");
    auto seg = tok.next();
    REQUIRE(seg);
    REQUIRE_FALSE(seg->style.fg);
  }
  SECTION("38;2 with only two components") {
    Tokenizer tok("\x1b[38;2;255;0mtext");
    auto seg = tok.next();
    REQUIRE(seg);
    REQUIRE_FALSE(seg->style.fg);
  }
}
