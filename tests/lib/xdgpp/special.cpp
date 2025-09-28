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

TEST_CASE("handle empty keys properly", XDGPP_GROUP) {
  auto entry = xdgpp::DesktopEntry::fromData(R"(
[Desktop Entry]
Categories=Network;WebBrowser;
Comment=
Exec=zen %U
GenericName=A sleek browser
Icon=/home/user/icons/Zen128.png
MimeType=text/html;text/xml;application/xhtml+xml;application/vnd.mozilla.xul+xml;text/mml;x-scheme-handler/http;x-scheme-handler/https;
Name=Zen Browser
Path=
StartupNotify=true
StartupWMClass=zen-beta
Terminal=false
TerminalOptions=
Type=Application
Version=1.0
X-KDE-SubstituteUID=false
X-KDE-Username=
)");

  REQUIRE(entry.comment() == "");
  REQUIRE(entry.workingDirectory() == "");

  auto exec = entry.exec();
  REQUIRE(exec == "zen %U");
}

TEST_CASE("localization test", XDGPP_GROUP) {
  auto entry = xdgpp::DesktopEntry::fromData(R"(
[Desktop Entry]
Name[ab]=Авидео
Name[af]=Video's
Name[ar]=فيديو
Name[as]=ভিডিঅ'সমূহ
Name[be]=Відэа
Name[be@latin]=Videa
Name[bg]=Видео клипове
Name[bn]=ভিডিও
Name[bn_IN]=ভিডিও
Name[bs]=Video
Name[ca]=Vídeos
Name[ca@valencia]=Vídeos
Name[crh]=Videolar
Name[cs]=Videa
Name[da]=Videoer
Name[de]=Videos
Name[el]=Βίντεο
Name[en_GB]=Videos
Name[en@shaw]=𐑝𐑦𐑛𐑰𐑴𐑟
Name[eo]=Videaĵoj
Name[es]=Vídeos
Name[et]=Videod
Name[eu]=Bideoak
Name[fa]=ویدیوها
Name[fi]=Videot
Name[fr]=Vidéos
Name[fur]=Videos
Name[ga]=Físeáin
Name[gd]=Videothan
Name[gl]=Vídeos
Name[gu]=વિડીઓ
Name[gv]=Feeshanyn
Name[he]=סרטים
Name[hi]=वीडियो
Name[hr]=Snimke
Name[hu]=Videók
Name[id]=Video
Name[ie]=Videos
Name[is]=Myndskeið
Name[it]=Video
Name[ja]=ビデオ
Name[ka]=ვიდეო
Name[kab]=Tividyutin
Name[kk]=Видеолар
Name[km]=វីដេអូ
Name[kn]=ವೀಡಿಯೋಗಳು
Name[ko]=동영상
Name[ky]=Видео
Name[lt]=Vaizdo įrašai
Name[lv]=Video
Name[mjw]=Videos
Name[ml]=വീഡിയോകള്‍
Name[mr]=व्हिडीओज्
Name[ms]=Video
Name[my]=ဗီဒီယိုများ
Name[nb]=Filmer
Name[ne]=भिडियो
Name[nl]=Video’s
Name[nn]=Videoar
Name[oc]=Vidèos
Name[or]=ଭିଡିଓ
Name[pa]=ਵਿਡੀਓ
Name[pl]=Filmy
Name[pt]=Vídeos
Name[pt_BR]=Vídeos
Name[ro]=Videouri
Name[ru]=Видео
Name[sk]=Videá
Name[sl]=Video
Name[sq]=Video
Name[sr]=Филмови
Name[sr@latin]=Filmovi
Name[sv]=Videoklipp
Name[ta]=வீடியோக்கள்
Name[te]=వీడియోలు
Name[tg]=Видеоҳо
Name[th]=วิดีโอ
Name[tr]=Videolar
Name[ug]=سىنلار
Name[uk]=Відео
Name[uz]=Videolar
Name[vi]=Xem phim
Name[zh_CN]=视频
Name[zh_HK]=影片
Name[zh_TW]=影片
Name[zu]=Amavidiyo
Name=Videos
Exec=totem %U
# Translators: Do NOT translate or transliterate this text (this is an icon file name)!
Icon=org.gnome.Totem
DBusActivatable=true
Terminal=false
Type=Application
Categories=GTK;GNOME;AudioVideo;Player;Video;
X-GNOME-DocPath=totem/totem.xml
StartupNotify=true
)",
                                             {.locale = xdgpp::Locale("en_US.utf8")});

  auto exec = entry.parseExec();
  REQUIRE(entry.name() == "Videos");
}
