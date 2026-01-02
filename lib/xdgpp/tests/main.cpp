#include <xdgpp/xdgpp.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("handle file without ending linefeed", XDGPP_GROUP) {
  constexpr const char *ENTRY = R"(
[Desktop Entry]
Name=My Application
Description=My Description)";

  xdgpp::DesktopEntryReader reader(ENTRY);
  auto grp = reader.group("Desktop Entry");
  auto name = grp->key("Name");
  auto desc = grp->key("Description");

  REQUIRE(grp);
  REQUIRE(name.has_value());
  REQUIRE(name->asString() == "My Application");
  REQUIRE(desc.has_value());
  REQUIRE(desc->asString() == "My Description");
};

TEST_CASE("ignore leading comments", XDGPP_GROUP) {
  constexpr const char *ENTRY = R"(#This is a comment line
  #    This is another comment
  #  And another comment
      # And some other comment
[Desktop Entry]
Name=My Application
# comment in the middle
    #    another comment
Description=My Description)";

  xdgpp::DesktopEntryReader reader(ENTRY);
};

TEST_CASE("handles simple kv without extra spacing", XDGPP_GROUP) {
  constexpr const char *ENTRY = R"(
[Desktop Entry]
Name=My Application
Description=My Description
)";

  xdgpp::DesktopEntryReader reader(ENTRY);
  auto grp = reader.group("Desktop Entry");

  REQUIRE(grp);
  REQUIRE(grp->name() == "Desktop Entry");
  REQUIRE(grp->rawKey("Name").has_value());
  REQUIRE(grp->rawKey("Name").has_value());
  REQUIRE(grp->rawKey("Description").has_value());
  REQUIRE_FALSE(grp->rawKey("Comment").has_value());
}

TEST_CASE("handles kv with extra linebreaks", XDGPP_GROUP) {
  constexpr const char *ENTRY = R"(

[Desktop Entry]

Name=My Application



Description=My Description


)";

  xdgpp::DesktopEntryReader reader(ENTRY);
  auto grp = reader.group("Desktop Entry");

  REQUIRE(grp);

  auto name = grp->key("Name");
  auto desc = grp->key("Description");

  REQUIRE(name.has_value());
  REQUIRE(name->asString() == "My Application");
  REQUIRE(desc.has_value());
  REQUIRE(desc->asString() == "My Description");
}

TEST_CASE("handles kv with inner spacing", XDGPP_GROUP) {
  constexpr const char *ENTRY = R"(

[Desktop Entry]

Name    =           My Application    
Description=      My Description         
Comment     =Test
)";

  xdgpp::DesktopEntryReader reader(ENTRY);
  auto grp = reader.group("Desktop Entry");

  REQUIRE(grp);
  auto name = grp->key("Name");
  auto desc = grp->key("Description");
  REQUIRE(name.has_value());
  REQUIRE(name->asString() == "My Application");
  REQUIRE(desc.has_value());
  REQUIRE(desc->asString() == "My Description");
}

TEST_CASE("handle string key with escape", XDGPP_GROUP) {
  constexpr const char *ENTRY = R"(
[Desktop Entry]
Name=My\sApplication
Description=My\tDescription
)";

  xdgpp::DesktopEntryReader reader(ENTRY);
  auto grp = reader.group("Desktop Entry");

  REQUIRE(grp);

  auto name = grp->key("Name");
  auto desc = grp->key("Description");
  REQUIRE(name.has_value());
  REQUIRE(name->asString() == "My Application");
  REQUIRE(desc.has_value());
  REQUIRE(desc->asString() == "My\tDescription");
};

TEST_CASE("do not trim escaped sequences", XDGPP_GROUP) {
  constexpr const char *ENTRY = R"(
[Desktop Entry]
Name=\sMy\sApplication\s
Description=\sMy\tDescription\s
)";

  xdgpp::DesktopEntryReader reader(ENTRY);
  auto grp = reader.group("Desktop Entry");

  REQUIRE(grp);
  auto name = grp->key("Name");
  auto desc = grp->key("Description");
  REQUIRE(name.has_value());
  REQUIRE(name->asString() == " My Application ");
  REQUIRE(desc.has_value());
  REQUIRE(desc->asString() == " My\tDescription ");
};

TEST_CASE("handles string list values", XDGPP_GROUP) {
  constexpr const char *ENTRY = R"(
[Desktop Entry]
Keywords=Keyword1;Keyword2
)";

  xdgpp::DesktopEntryReader reader(ENTRY);
  auto grp = reader.group("Desktop Entry");

  REQUIRE(grp);

  auto kw = grp->key("Keywords");

  REQUIRE(kw.has_value());
  auto kws = kw->asStringList();
  REQUIRE(kws.size() == 2);
  REQUIRE(kws[0] == "Keyword1");
  REQUIRE(kws[1] == "Keyword2");
};

TEST_CASE("handle escaped sequences in string list value", XDGPP_GROUP) {
  constexpr const char *ENTRY = R"(
[Desktop Entry]
Keywords=K\se\sy\;word1;Keyword2\;
)";

  xdgpp::DesktopEntryReader reader(ENTRY);
  auto grp = reader.group("Desktop Entry");

  REQUIRE(grp);

  auto kw = grp->key("Keywords");

  REQUIRE(kw.has_value());
  auto kws = kw->asStringList();
  REQUIRE(kws.size() == 2);
  REQUIRE(kws[0] == "K e y;word1");
  REQUIRE(kws[1] == "Keyword2;");
}

TEST_CASE("handle boolean values", XDGPP_GROUP) {
  constexpr const char *ENTRY = R"(
[Desktop Entry]
Hidden=true
NoDisplay=false
)";

  xdgpp::DesktopEntryReader reader(ENTRY);
  auto grp = reader.group("Desktop Entry");

  REQUIRE(grp);

  auto hidden = grp->key("Hidden");
  auto noDisplay = grp->key("NoDisplay");

  REQUIRE(hidden.has_value());
  REQUIRE(hidden->asBoolean());

  REQUIRE(noDisplay.has_value());
  REQUIRE(!noDisplay->asBoolean());
}

TEST_CASE("handle numeric values", XDGPP_GROUP) {
  constexpr const char *ENTRY = R"(
[Desktop Entry]
WrongAnswer=21
Answer=42
Invalid=0qwoiikqw
)";

  xdgpp::DesktopEntryReader reader(ENTRY);
  auto grp = reader.group("Desktop Entry");

  REQUIRE(grp);

  auto wrongAnswer = grp->key("WrongAnswer");
  auto answer = grp->key("Answer");
  auto invalid = grp->key("Invalid");

  REQUIRE((wrongAnswer.has_value() && wrongAnswer->asNumber() == 21));
  REQUIRE((answer.has_value() && answer->asNumber() == 42));
  REQUIRE((invalid.has_value() && invalid->asNumber() == 0));
}

TEST_CASE("handle localized keys", XDGPP_GROUP) {
  constexpr const char *ENTRY = R"(
[Desktop Entry]
Name[en_US]=American Name
Name[en_GB@utf8]=British Name
)";

  xdgpp::DesktopEntryReader reader(ENTRY, {.locale = xdgpp::Locale("en_US")});
  auto grp = reader.group("Desktop Entry");

  REQUIRE(grp);
  auto name = grp->key("Name");

  REQUIRE(name.has_value());
  REQUIRE(name->asString() == "American Name");
}
