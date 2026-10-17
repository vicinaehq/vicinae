#include <catch2/catch_test_macros.hpp>
#include <QMimeData>
#include "services/clipboard/clipboard-mime.hpp"

// NOLINTBEGIN(bugprone-throwing-static-initialization)
TEST_CASE("selection mime data keeps plain text separate from rich text offers") {
  ClipboardSelection selection;
  selection.offers = {
      {.mimeType = "text/html", .data = "<b>402-5329634</b>"},
      {.mimeType = "text/plain", .data = "402-5329634"},
      {.mimeType = "text/plain;charset=utf-8", .data = "402-5329634"},
      {.mimeType = "text/richtext", .data = "richtext payload"},
      {.mimeType = "text/rtf", .data = R"({\rtf1\ansi 402-5329634})"},
  };

  const auto data = Clipboard::mimeDataForSelection(selection);

  REQUIRE(data->text() == "402-5329634");
  REQUIRE(data->data("text/plain") == "402-5329634");
  REQUIRE(data->data("text/plain;charset=utf-8") == "402-5329634");
  REQUIRE(data->data("text/html") == "<b>402-5329634</b>");
  REQUIRE(data->data("text/richtext") == "richtext payload");
  REQUIRE(data->data("text/rtf") == R"({\rtf1\ansi 402-5329634})");
}

TEST_CASE("selection mime data uses X11 text targets as plain text") {
  ClipboardSelection selection;
  selection.offers = {{.mimeType = "UTF8_STRING", .data = "hello"}};

  const auto data = Clipboard::mimeDataForSelection(selection);

  REQUIRE(data->text() == "hello");
  REQUIRE(data->data("UTF8_STRING") == "hello");
}

TEST_CASE("selection mime data derives plain text from html when no plain text offer exists") {
  ClipboardSelection selection;
  selection.offers = {{.mimeType = "text/html", .data = "<p>hello <b>world</b></p>"}};

  const auto data = Clipboard::mimeDataForSelection(selection);

  REQUIRE(data->text() == "hello world");
  REQUIRE(data->data("text/html") == "<p>hello <b>world</b></p>");
}

TEST_CASE("selection mime data keeps the original text/plain bytes") {
  ClipboardSelection selection;
  selection.offers = {
      {.mimeType = "text/plain", .data = "plain variant"},
      {.mimeType = "text/plain;charset=utf-8", .data = "utf-8 variant"},
  };

  const auto data = Clipboard::mimeDataForSelection(selection);

  REQUIRE(data->data("text/plain") == "plain variant");
  REQUIRE(data->data("text/plain;charset=utf-8") == "utf-8 variant");
}

TEST_CASE("selection mime data synthesizes text/plain from the utf-8 variant") {
  ClipboardSelection selection;
  selection.offers = {
      {.mimeType = "text/plain", .data = ""},
      {.mimeType = "text/plain;charset=utf-8", .data = "hello"},
  };

  const auto data = Clipboard::mimeDataForSelection(selection);

  REQUIRE(data->text() == "hello");
}

TEST_CASE("selection mime data does not advertise empty offers") {
  ClipboardSelection selection;
  selection.offers = {
      {.mimeType = "text/html", .data = ""},
      {.mimeType = "text/plain", .data = "hello"},
  };

  const auto data = Clipboard::mimeDataForSelection(selection);

  REQUIRE_FALSE(data->hasFormat("text/html"));
  REQUIRE(data->text() == "hello");
}

TEST_CASE("selection mime data keeps the first offer of a duplicated mime type") {
  ClipboardSelection selection;
  selection.offers = {
      {.mimeType = "text/plain", .data = "first"},
      {.mimeType = "text/plain", .data = "second"},
  };

  const auto data = Clipboard::mimeDataForSelection(selection);

  REQUIRE(data->text() == "first");
}

TEST_CASE("selection mime data strips object placeholders from html-derived text") {
  ClipboardSelection selection;
  selection.offers = {{.mimeType = "text/html", .data = R"(<p><img src="x.png">caption</p>)"}};

  const auto data = Clipboard::mimeDataForSelection(selection);

  REQUIRE(data->text() == "caption");
}

TEST_CASE("selection mime data does not set text when html has none") {
  ClipboardSelection selection;
  selection.offers = {{.mimeType = "text/html", .data = R"(<img src="x.png">)"}};

  const auto data = Clipboard::mimeDataForSelection(selection);

  REQUIRE_FALSE(data->hasText());
  REQUIRE(data->hasFormat("text/html"));
}

TEST_CASE("selection mime data preserves images alongside text") {
  ClipboardSelection selection;
  selection.offers = {
      {.mimeType = "image/png", .data = "png bytes"},
      {.mimeType = "text/plain", .data = "caption"},
  };

  const auto data = Clipboard::mimeDataForSelection(selection);

  REQUIRE(data->data("image/png") == "png bytes");
  REQUIRE(data->text() == "caption");
}
// NOLINTEND(bugprone-throwing-static-initialization)
