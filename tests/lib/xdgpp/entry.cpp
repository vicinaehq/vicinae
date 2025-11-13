#include <xdgpp/xdgpp.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string_view>

using namespace xdgpp;

static constexpr const char *GROUP = XDGPP_GROUP;

TEST_CASE("parse firefox desktop file") {
  auto firefox = R"(
[Desktop Entry]
Version=1.0
Name=Mozilla Firefox (bin)
GenericName=Web Browser
Comment=Browse the Web
Exec=firefox-bin --name=firefox-bin %u
Icon=firefox-bin
Terminal=false
Type=Application
MimeType=application/pdf;application/vnd.mozilla.xul+xml;application/xhtml+xml;text/html;text/mml;text/xml;x-scheme-handler/http;x-scheme-handler/https;
StartupNotify=true
Categories=Network;WebBrowser;
Keywords=web;browser;internet;
Actions=new-window;new-private-window;
StartupWMClass=firefox
)";

  auto file = DesktopEntry::fromData(firefox);

  REQUIRE(file.isValid());
  REQUIRE(!file.errorMessage().has_value());
  REQUIRE(file.isApplication());
  REQUIRE(file.icon() == "firefox-bin");
  REQUIRE(file.name() == "Mozilla Firefox (bin)");
  REQUIRE(std::ranges::find(file.mimes(), "application/pdf") != file.mimes().end());
  REQUIRE(file.comment() == "Browse the Web");
  REQUIRE(file.genericName() == "Web Browser");
  REQUIRE(file.startupWMClass() == "firefox");
  REQUIRE(file.deleted() == false);
  REQUIRE(file.noDisplay() == false);

  auto &kws = file.keywords();

  // web;browser;internet
  REQUIRE(kws.size() == 3);
  REQUIRE(kws[0] == "web");
  REQUIRE(kws[1] == "browser");
  REQUIRE(kws[2] == "internet");
}

TEST_CASE("should choose the localized entry as name") {
  auto data = R"(
[Desktop Entry]
Name=Unlocalized Name
Name[en_US]=Name US
Name[en_CA.utf8]=Name CA
Name[en_GB.utf8]=Name GB
Name[fr.utf8]=Name FR
Name[fr_CA.utf8]=Name FR_CA
)";

  {
    auto file = DesktopEntry::fromData(data, {.locale = Locale("en_US.utf8")});
    REQUIRE(file.name() == "Name US");
  }

  {
    auto file = DesktopEntry::fromData(data, {.locale = Locale("en_CA")});
    REQUIRE(file.name() == "Name CA");
  }

  {
    auto file = DesktopEntry::fromData(data, {.locale = Locale("fr")});
    REQUIRE(file.name() == "Name FR");
  }

  {
    auto file = DesktopEntry::fromData(data, {.locale = Locale("es_ES")});
    REQUIRE(file.name() == "Unlocalized Name");
  }
}

TEST_CASE("should be invalid if entry has no name") {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Icon=Stuff
Exec=some-exec
	)");

  REQUIRE_FALSE(file.isValid());
  REQUIRE(file.errorMessage().has_value());
}

TEST_CASE("should be invalid if file does not exist") {
  auto file = DesktopEntry::fromFile("/fake/ass/path");

  REQUIRE_FALSE(file.isValid());
  REQUIRE(file.errorMessage().has_value());
}

TEST_CASE("should be invalid if data is empty") {
  auto file = DesktopEntry::fromData("");

  REQUIRE_FALSE(file.isValid());
  REQUIRE(file.errorMessage().has_value());
}

TEST_CASE("should skip invalid entries") {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name: MyName
Comment=MyComment
Answer  42
Keywords=test;fun
  )");

  REQUIRE_FALSE(file.isValid()); // name is ignored
  REQUIRE(file.comment() == "MyComment");
  REQUIRE(file.keywords().at(0) == "test");
  REQUIRE(file.keywords().at(1) == "fun");
}

TEST_CASE("should parse categories", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=Test
Categories=1;2;3;4
  )");

  REQUIRE(file.isValid());
  REQUIRE(file.categories().size() == 4);
  REQUIRE(file.categories()[0] == "1");
  REQUIRE(file.categories()[1] == "2");
  REQUIRE(file.categories()[2] == "3");
  REQUIRE(file.categories()[3] == "4");
}

TEST_CASE("handle copy", GROUP) {
  auto file = DesktopEntry::fromData(R"(
Name=Test
Comment=Some comment
  )");
  auto copy = file;

  REQUIRE(copy.name() == file.name());
}

TEST_CASE("should ignore keys not within a group", GROUP) {
  auto file = DesktopEntry::fromData(R"(
Name=Test
Comment=Some comment
  )");

  REQUIRE(!file.isValid());
}

TEST_CASE("should parse simple unquoted exec", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec=program test --flag true --other-flag false
  )");

  REQUIRE(file.exec().has_value());
  auto exec = file.parseExec();
  REQUIRE(exec.at(0) == "program");
  REQUIRE(exec.at(1) == "test");
  REQUIRE(exec.at(2) == "--flag");
  REQUIRE(exec.at(3) == "true");
  REQUIRE(exec.at(4) == "--other-flag");
  REQUIRE(exec.at(5) == "false");
}

TEST_CASE("should parse one big quoted exec", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = "program test --flag true --other-flag false"
  )");

  auto exec = file.parseExec();
  REQUIRE(exec.size() == 1);
  REQUIRE(exec.at(0) == "program test --flag true --other-flag false");
}

TEST_CASE("should handle escaped quotes inside quoted exec", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = program --name "This is \\"quoted\\""
  )");

  auto exec = file.parseExec();
  REQUIRE(exec.size() == 3);
  REQUIRE(exec.at(0) == "program");
  REQUIRE(exec.at(1) == "--name");
  REQUIRE(exec.at(2) == "This is \"quoted\"");
}

TEST_CASE("should concatenate consecutive quoted strings", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = program """this is"" quoted"""
  )");

  auto exec = file.parseExec();
  REQUIRE(exec.at(0) == "program");
  REQUIRE(exec.at(1) == "this is quoted");
}

TEST_CASE("should handle blackslash escaping in quoted string", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = program "\\\\hello\\\\"
  )");

  auto exec = file.parseExec();
  REQUIRE(exec.at(0) == "program");
  REQUIRE(exec.at(1) == "\\hello\\");
}

TEST_CASE("should ignore empty concatenation of quoted strings", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = program """""""""""""" true_arg
  )");

  auto exec = file.parseExec();
  REQUIRE(exec.size() == 2);
  REQUIRE(exec.at(0) == "program");
  REQUIRE(exec.at(1) == "true_arg");
}

TEST_CASE("handle many arguments", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = a  b   c   d   e    f      g   
  )");

  auto exec = file.parseExec();

  size_t i = 0;
  for (const auto &s : std::string_view("abcdefg")) {
    REQUIRE(exec.at(i) == std::string{s});
    ++i;
  }
}

TEST_CASE("handle percent escaping in exec key", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = firefox %%u %%%% 100%%
  )");

  auto exec = file.parseExec();

  REQUIRE(exec.size() == 4);
  REQUIRE(exec.at(0) == "firefox");
  REQUIRE(exec.at(1) == "%u");
  REQUIRE(exec.at(2) == "%%");
  REQUIRE(exec.at(3) == "100%");
}

TEST_CASE("field code should be stripped if no uri is provided", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = firefox %u
  )");

  auto exec = file.parseExec();

  REQUIRE(exec.size() == 1);
  REQUIRE(exec.at(0) == "firefox");
}

TEST_CASE("%u field code should expand to provided uri", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = firefox %u
  )");

  auto exec = file.parseExec({"expanded value"});

  REQUIRE(exec.size() == 2);
  REQUIRE(exec.at(0) == "firefox");
  REQUIRE(exec.at(1) == "expanded value");
}

TEST_CASE("%i field code should expand to icon", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Icon=/path/to/icon.png
Exec = firefox %i
  )");

  auto exec = file.parseExec();

  REQUIRE(exec.at(0) == "firefox");
  REQUIRE(exec.at(1) == "--icon");
  REQUIRE(exec.at(2) == "/path/to/icon.png");
}

TEST_CASE("%i field code should expand to nothing if there is no icon", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = firefox %i some_arg
  )");

  auto exec = file.parseExec();

  REQUIRE(exec.at(0) == "firefox");
  REQUIRE(exec.at(1) == "some_arg");
  REQUIRE(exec.size() == 2);
}

TEST_CASE("%c field code should expand to translated name", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = firefox --name %c
  )");

  auto exec = file.parseExec();

  REQUIRE(exec.at(0) == "firefox");
  REQUIRE(exec.at(1) == "--name");
  REQUIRE(exec.at(2) == "MyFile");
  REQUIRE(exec.size() == 3);
}

TEST_CASE("%F field code should expand to a list of files", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = firefox %F --private-window
  )");

  auto exec = file.parseExec({"file1.png", "file2.png", "file3.png"});

  REQUIRE(exec.at(0) == "firefox");
  REQUIRE(exec.at(1) == "file1.png");
  REQUIRE(exec.at(2) == "file2.png");
  REQUIRE(exec.at(3) == "file3.png");
  REQUIRE(exec.at(4) == "--private-window");
  REQUIRE(exec.size() == 5);
}

TEST_CASE("%U field code should expand to a list of uris (or files)", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = firefox %U --private-window
  )");

  auto exec = file.parseExec({"file://file1.png", "file://file2.png", "file://file3.png"});

  REQUIRE(exec.at(0) == "firefox");
  REQUIRE(exec.at(1) == "file://file1.png");
  REQUIRE(exec.at(2) == "file://file2.png");
  REQUIRE(exec.at(3) == "file://file3.png");
  REQUIRE(exec.at(4) == "--private-window");
  REQUIRE(exec.size() == 5);
}

TEST_CASE("Deprecated field codes (%d %D %n %N %v %m) should be removed", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = firefox %d %D %n --private-window %v %m
  )");

  auto exec = file.parseExec({"file://file1.png"});

  REQUIRE(exec.at(0) == "firefox");
  REQUIRE(exec.at(1) == "--private-window");
  REQUIRE(exec.size() == 2);
};

TEST_CASE("Parse desktop actions", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = firefox
Actions = open-private;

[Desktop Action open-private]
Name=Open In Private Window
Exec=firefox %U
Icon=firefox
  )");

  REQUIRE(file.actions().size() == 1);

  auto &action = file.actions().at(0);

  REQUIRE(action.id() == "open-private");
  REQUIRE(action.name() == "Open In Private Window");
  REQUIRE(action.exec() == "firefox %U");
  REQUIRE(action.icon() == "firefox");
  REQUIRE(action.parseExec({"file://test"}).at(1) == "file://test");
};

TEST_CASE("Should not parse action not listed in Actions key", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = firefox

[Desktop Action open-private]
Name=Open In Private Window
Exec=firefox %U
Icon=firefox
  )");

  REQUIRE(file.actions().empty());
};

TEST_CASE("should support SingleMainWindow key", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = firefox
SingleMainWindow = true

  )");

  REQUIRE(file.singleMainWindow());
};

TEST_CASE("should support TryExec key", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = firefox
TryExec = try-firefox

  )");

  REQUIRE(file.tryExec() == "try-firefox");
};

TEST_CASE("should support StartupWMClass key", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = firefox
StartupWMClass = firefox-class

  )");

  REQUIRE(file.startupWMClass() == "firefox-class");
}

TEST_CASE("should support Terminal key", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=Neovim
Exec = nvim
Terminal = true

  )");

  REQUIRE(file.terminal());
}

TEST_CASE("should support Path key", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = firefox
Path = /opt/firefox-wd

  )");

  REQUIRE(file.workingDirectory() == "/opt/firefox-wd");
}

TEST_CASE("should support OnlyShowIn key", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = firefox
OnlyShowIn = Gnome;KDE;
)");

  REQUIRE(file.onlyShowIn().at(0) == "Gnome");
  REQUIRE(file.onlyShowIn().at(1) == "KDE");
}

TEST_CASE("should support NotShowIn key", GROUP) {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = firefox
NotShowIn  = Gnome;KDE;
)");

  REQUIRE(file.notShowIn().at(0) == "Gnome");
  REQUIRE(file.notShowIn().at(1) == "KDE");
}

TEST_CASE("should force append one uri if no field code was expanded") {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = firefox
)");

  auto exec = file.parseExec({"https://example.com"}, true);

  REQUIRE(exec.at(0) == "firefox");
  REQUIRE(exec.at(1) == "https://example.com");
}

TEST_CASE("should force append many uris if no field code was expanded") {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = firefox
)");

  auto exec = file.parseExec({"1", "2", "3"}, true);

  REQUIRE(exec.at(0) == "firefox");
  REQUIRE(exec.at(1) == "1");
  REQUIRE(exec.at(2) == "2");
  REQUIRE(exec.at(3) == "3");
}

TEST_CASE("should not force append many uris if a field code was expanded") {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Exec = firefox %f
)");

  auto exec = file.parseExec({"1", "2", "3"}, true);

  REQUIRE(exec.at(0) == "firefox");
  REQUIRE(exec.at(1) == "1");
  REQUIRE(exec.size() == 2);
}

TEST_CASE("Should parse URL for link entries") {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Type=Link
URL=https://vicinae.com
)");

  REQUIRE(file.isValid());
  REQUIRE(file.url() == "https://vicinae.com");
}

TEST_CASE("Should flag a link entry without an URL as invalid") {
  auto file = DesktopEntry::fromData(R"(
[Desktop Entry]
Name=MyFile
Type=Link
)");

  REQUIRE(!file.isValid());
}
