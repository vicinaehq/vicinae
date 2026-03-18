#include <catch2/catch_test_macros.hpp>
#include "ansi/ansi.hpp"

using namespace ansi;

TEST_CASE("plain text is html-escaped") {
  REQUIRE(to_html("hello") == "hello");
  REQUIRE(to_html("<b>bold</b>") == "&lt;b&gt;bold&lt;/b&gt;");
  REQUIRE(to_html("a & b") == "a &amp; b");
  REQUIRE(to_html("a \"b\" c") == "a &quot;b&quot; c");
}

TEST_CASE("empty input produces empty output") { REQUIRE(to_html("") == ""); }

TEST_CASE("foreground color produces span") {
  Palette pal;
  auto html = to_html("\x1b[31mhello", pal);
  REQUIRE(html == "<span style=\"color:#cc0000\">hello</span>");
}

TEST_CASE("background color produces span") {
  Palette pal;
  auto html = to_html("\x1b[42mhello", pal);
  REQUIRE(html == "<span style=\"background:#4e9a06\">hello</span>");
}

TEST_CASE("bold and italic produce correct css") {
  auto html = to_html("\x1b[1;3mtext");
  REQUIRE(html == "<span style=\"font-weight:bold;font-style:italic\">text</span>");
}

TEST_CASE("combined text-decoration values") {
  auto html = to_html("\x1b[4;9;53mtext");
  REQUIRE(html == "<span style=\"text-decoration:underline line-through overline\">text</span>");
}

TEST_CASE("reset mid-text closes span") {
  Palette pal;
  auto html = to_html("\x1b[31mred\x1b[0mnormal", pal);
  REQUIRE(html == "<span style=\"color:#cc0000\">red</span>normal");
}

TEST_CASE("style change produces new span") {
  Palette pal;
  auto html = to_html("\x1b[31mred\x1b[32mgreen", pal);
  REQUIRE(html == "<span style=\"color:#cc0000\">red</span><span style=\"color:#4e9a06\">green</span>");
}

TEST_CASE("custom palette is used") {
  Palette pal;
  pal.standard[1] = "#ff0000";
  auto html = to_html("\x1b[31mtext", pal);
  REQUIRE(html == "<span style=\"color:#ff0000\">text</span>");
}

TEST_CASE("truecolor in html") {
  auto html = to_html("\x1b[38;2;255;128;0mtext");
  REQUIRE(html == "<span style=\"color:#ff8000\">text</span>");
}

TEST_CASE("256-color cube in html") {
  // index 196 = rgb(5,0,0) in 6x6x6 cube → #ff0000
  auto html = to_html("\x1b[38;5;196mtext");
  REQUIRE(html == "<span style=\"color:#ff0000\">text</span>");
}

TEST_CASE("256-color grayscale in html") {
  // index 232 = first grayscale = rgb(8,8,8)
  auto html = to_html("\x1b[38;5;232mtext");
  REQUIRE(html == "<span style=\"color:#080808\">text</span>");
}

TEST_CASE("256-color maps low indices to palette") {
  Palette pal;
  // index 1 should use standard[1]
  auto html = to_html("\x1b[38;5;1mtext", pal);
  REQUIRE(html == "<span style=\"color:#cc0000\">text</span>");

  // index 9 should use bright[1]
  html = to_html("\x1b[38;5;9mtext", pal);
  REQUIRE(html == "<span style=\"color:#ef2929\">text</span>");
}

TEST_CASE("html content between spans is escaped") {
  auto html = to_html("\x1b[31m<script>\x1b[0m");
  REQUIRE(html == "<span style=\"color:#cc0000\">&lt;script&gt;</span>");
}

TEST_CASE("dim produces opacity") {
  auto html = to_html("\x1b[2mtext");
  REQUIRE(html == "<span style=\"opacity:0.5\">text</span>");
}

TEST_CASE("hidden produces visibility hidden") {
  auto html = to_html("\x1b[8mtext");
  REQUIRE(html == "<span style=\"visibility:hidden\">text</span>");
}

TEST_CASE("same style across segments does not reopen span") {
  Palette pal;
  // Two consecutive red sequences — should merge into one span
  auto html = to_html("\x1b[31mhello\x1b[31m world", pal);
  REQUIRE(html == "<span style=\"color:#cc0000\">hello world</span>");
}

TEST_CASE("resolve_color 256 palette boundary values") {
  Palette pal;

  // index 15 = last bright color
  REQUIRE(resolve_color(Color::palette(15), pal) == pal.bright[7]);

  // index 16 = first cube color (0,0,0)
  REQUIRE(resolve_color(Color::palette(16), pal) == "#000000");

  // index 231 = last cube color (5,5,5) → #ffffff
  REQUIRE(resolve_color(Color::palette(231), pal) == "#ffffff");

  // index 255 = last grayscale → 8 + 23*10 = 238
  REQUIRE(resolve_color(Color::palette(255), pal) == "#eeeeee");
}
