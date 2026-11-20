#include <filesystem>
#include <ranges>
#include <string>
#include <string_view>
#include <system_error>
#include <QDebug>
#include <QRegularExpression>
#include <cpp-pinyin2/core/Engine.h>
#include <cpp-pinyin2/ext/Registry.h>
#include "app-pinyin-keywords.hpp"
#include "common/common.hpp"

void appendAppPinyinKeywords(QStringView name, std::vector<QString> &keywords) {
  static const QRegularExpression HAN(QStringLiteral(R"(\p{Han})"));
  if (!name.contains(HAN)) return;

  using namespace cpp_pinyin::v2;
  static const auto PINYIN = [] {
    const Registry registry;
    const auto appDir = vicinae::selfPath().parent_path();
    auto dictRoot = (appDir / VICINAE_PINYIN_DIR_REL).lexically_normal();
#ifdef Q_OS_LINUX
    // Developer builds and the AppImage's bin/ copy sit one level closer to share/.
    const auto binDict = appDir.parent_path() / "share/vicinae/pinyin";
    std::error_code ec;
    if (!std::filesystem::is_directory(dictRoot, ec) && std::filesystem::is_directory(binDict, ec))
      dictRoot = binDict;
#endif
    auto engine = Engine::open(registry, dictRoot, "mandarin");
    if (!engine) qWarning() << "Could not load the pinyin dictionary:" << engine.error().detail;
    return engine;
  }();
  if (!PINYIN) return;

  const auto utf8 = name.toString().simplified().toUtf8();
  const std::string_view text = utf8;

  std::string full;
  std::string initials;
  full.reserve(text.size());
  initials.reserve(text.size());
  // The transliterator omits spaces; keep title word boundaries for fuzzy matching.
  for (const auto word : text | std::views::split(' ')) {
    const auto result = PINYIN.value().query(std::string_view{word}, {.style = MandarinStyle::NORMAL});
    if (!result) {
      qWarning() << "Could not transliterate application name:" << result.error().detail;
      return;
    }

    if (!full.empty()) {
      full += ' ';
      initials += ' ';
    }
    for (const auto &part : result.value()) {
      full += part.rendered;
      if (part.text == part.rendered) {
        initials += part.rendered;
      } else if (!part.rendered.empty()) {
        initials += part.rendered.front();
      }
    }
  }
  if (full == text) return;

  keywords.reserve(keywords.size() + 2);
  keywords.emplace_back(QString::fromStdString(full));
  if (initials != full) keywords.emplace_back(QString::fromStdString(initials));
}
