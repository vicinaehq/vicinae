#include <xdgpp/xdgpp.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/internal/catch_decomposer.hpp>

// https://github.com/vicinaehq/vicinae/discussions/145
TEST_CASE("wine auto-generated", XDGPP_GROUP) {
  auto entry = xdgpp::DesktopEntry::fromData(R"(
[Desktop Entry]
Name=한워드 2022
Exec=env WINEPREFIX="/home/quadratech/.wine" wine C:\\\\users\\\\quadratech\\\\AppData\\\\Roaming\\\\Microsoft\\\\Windows\\\\Start\\ Menu\\\\Programs\\\\한워드\\ 2022.lnk
Type=Application
StartupNotify=true
Comment=호환성이 높은 워드프로세서 문서를 만듭니다.
Path=/home/quadratech/.wine/dosdevices/c:/Program Files (x86)/Hnc/Office 2022/HOffice120/Bin/
Icon=ACEB_HWord.0
StartupWMClass=hword.exe
	)");

  auto exec = entry.parseExec();
  REQUIRE(exec.at(0) == "env");
  REQUIRE(exec.at(1) == "WINEPREFIX=/home/quadratech/.wine");
  REQUIRE(exec.at(2) == "wine");
  REQUIRE(
      exec.at(3) ==
      "C:\\users\\quadratech\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\한워드 2022.lnk");
}

// this is not spec-compliant but we support it
// this may break other desktop files, though, we will have to see if that's a problem or not
// as long as our tests are passing...
TEST_CASE("support quoting with single quotes", XDGPP_GROUP) {
  auto entry = xdgpp::DesktopEntry::fromData(R"(
[Desktop Entry]
Type=Application
Name=YouTube Music
Exec=vivaldi --app='https://music.youtube.com'
Icon=ftwa-youtube-music
Terminal=false
StartupNotify=true
StartupWMClass=ftwa-youtube-music
)");

  auto exec = entry.parseExec();
  REQUIRE(exec.at(0) == "vivaldi");
  REQUIRE(exec.at(1) == "--app=https://music.youtube.com");
}
