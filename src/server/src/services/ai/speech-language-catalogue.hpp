#pragma once
#include <algorithm>
#include <array>
#include <span>
#include <string>
#include <string_view>
#include <QCoreApplication>
#include <QLocale>
#include <QString>

struct SpeechLanguage {
  std::string_view code;
  const char *name;
  std::string_view nativeName;
};

namespace SpeechLanguageCatalogue {

constexpr auto TRANSLATION_CONTEXT = "SpeechLanguageCatalogue";

// Registered with lupdate as an alias of QT_TRANSLATE_NOOP in the translations target.
#define SPEECH_LANGUAGE_TR(text) QT_TRANSLATE_NOOP("SpeechLanguageCatalogue", text)

// clang-format off
constexpr auto ENTRIES = std::to_array<SpeechLanguage>({
  {"en", SPEECH_LANGUAGE_TR("English"), "English"},
  {"zh", SPEECH_LANGUAGE_TR("Chinese"), "中文"},
  {"de", SPEECH_LANGUAGE_TR("German"), "Deutsch"},
  {"es", SPEECH_LANGUAGE_TR("Spanish"), "Español"},
  {"ru", SPEECH_LANGUAGE_TR("Russian"), "Русский"},
  {"ko", SPEECH_LANGUAGE_TR("Korean"), "한국어"},
  {"fr", SPEECH_LANGUAGE_TR("French"), "Français"},
  {"ja", SPEECH_LANGUAGE_TR("Japanese"), "日本語"},
  {"pt", SPEECH_LANGUAGE_TR("Portuguese"), "Português"},
  {"tr", SPEECH_LANGUAGE_TR("Turkish"), "Türkçe"},
  {"pl", SPEECH_LANGUAGE_TR("Polish"), "Polski"},
  {"ca", SPEECH_LANGUAGE_TR("Catalan"), "Català"},
  {"nl", SPEECH_LANGUAGE_TR("Dutch"), "Nederlands"},
  {"ar", SPEECH_LANGUAGE_TR("Arabic"), "العربية"},
  {"sv", SPEECH_LANGUAGE_TR("Swedish"), "Svenska"},
  {"it", SPEECH_LANGUAGE_TR("Italian"), "Italiano"},
  {"id", SPEECH_LANGUAGE_TR("Indonesian"), "Bahasa Indonesia"},
  {"hi", SPEECH_LANGUAGE_TR("Hindi"), "हिन्दी"},
  {"fi", SPEECH_LANGUAGE_TR("Finnish"), "Suomi"},
  {"vi", SPEECH_LANGUAGE_TR("Vietnamese"), "Tiếng Việt"},
  {"he", SPEECH_LANGUAGE_TR("Hebrew"), "עברית"},
  {"uk", SPEECH_LANGUAGE_TR("Ukrainian"), "Українська"},
  {"el", SPEECH_LANGUAGE_TR("Greek"), "Ελληνικά"},
  {"ms", SPEECH_LANGUAGE_TR("Malay"), "Bahasa Melayu"},
  {"cs", SPEECH_LANGUAGE_TR("Czech"), "Čeština"},
  {"ro", SPEECH_LANGUAGE_TR("Romanian"), "Română"},
  {"da", SPEECH_LANGUAGE_TR("Danish"), "Dansk"},
  {"hu", SPEECH_LANGUAGE_TR("Hungarian"), "Magyar"},
  {"ta", SPEECH_LANGUAGE_TR("Tamil"), "தமிழ்"},
  {"no", SPEECH_LANGUAGE_TR("Norwegian"), "Norsk"},
  {"th", SPEECH_LANGUAGE_TR("Thai"), "ไทย"},
  {"ur", SPEECH_LANGUAGE_TR("Urdu"), "اردو"},
  {"hr", SPEECH_LANGUAGE_TR("Croatian"), "Hrvatski"},
  {"bg", SPEECH_LANGUAGE_TR("Bulgarian"), "Български"},
  {"lt", SPEECH_LANGUAGE_TR("Lithuanian"), "Lietuvių"},
  {"la", SPEECH_LANGUAGE_TR("Latin"), "Latina"},
  {"mi", SPEECH_LANGUAGE_TR("Maori"), "Te Reo Māori"},
  {"ml", SPEECH_LANGUAGE_TR("Malayalam"), "മലയാളം"},
  {"cy", SPEECH_LANGUAGE_TR("Welsh"), "Cymraeg"},
  {"sk", SPEECH_LANGUAGE_TR("Slovak"), "Slovenčina"},
  {"te", SPEECH_LANGUAGE_TR("Telugu"), "తెలుగు"},
  {"fa", SPEECH_LANGUAGE_TR("Persian"), "فارسی"},
  {"lv", SPEECH_LANGUAGE_TR("Latvian"), "Latviešu"},
  {"bn", SPEECH_LANGUAGE_TR("Bengali"), "বাংলা"},
  {"sr", SPEECH_LANGUAGE_TR("Serbian"), "Српски"},
  {"az", SPEECH_LANGUAGE_TR("Azerbaijani"), "Azərbaycan"},
  {"sl", SPEECH_LANGUAGE_TR("Slovenian"), "Slovenščina"},
  {"kn", SPEECH_LANGUAGE_TR("Kannada"), "ಕನ್ನಡ"},
  {"et", SPEECH_LANGUAGE_TR("Estonian"), "Eesti"},
  {"mk", SPEECH_LANGUAGE_TR("Macedonian"), "Македонски"},
  {"br", SPEECH_LANGUAGE_TR("Breton"), "Brezhoneg"},
  {"eu", SPEECH_LANGUAGE_TR("Basque"), "Euskara"},
  {"is", SPEECH_LANGUAGE_TR("Icelandic"), "Íslenska"},
  {"hy", SPEECH_LANGUAGE_TR("Armenian"), "Հայերեն"},
  {"ne", SPEECH_LANGUAGE_TR("Nepali"), "नेपाली"},
  {"mn", SPEECH_LANGUAGE_TR("Mongolian"), "Монгол"},
  {"bs", SPEECH_LANGUAGE_TR("Bosnian"), "Bosanski"},
  {"kk", SPEECH_LANGUAGE_TR("Kazakh"), "Қазақша"},
  {"sq", SPEECH_LANGUAGE_TR("Albanian"), "Shqip"},
  {"sw", SPEECH_LANGUAGE_TR("Swahili"), "Kiswahili"},
  {"gl", SPEECH_LANGUAGE_TR("Galician"), "Galego"},
  {"mr", SPEECH_LANGUAGE_TR("Marathi"), "मराठी"},
  {"pa", SPEECH_LANGUAGE_TR("Punjabi"), "ਪੰਜਾਬੀ"},
  {"si", SPEECH_LANGUAGE_TR("Sinhala"), "සිංහල"},
  {"km", SPEECH_LANGUAGE_TR("Khmer"), "ខ្មែរ"},
  {"sn", SPEECH_LANGUAGE_TR("Shona"), "chiShona"},
  {"yo", SPEECH_LANGUAGE_TR("Yoruba"), "Yorùbá"},
  {"so", SPEECH_LANGUAGE_TR("Somali"), "Soomaali"},
  {"af", SPEECH_LANGUAGE_TR("Afrikaans"), "Afrikaans"},
  {"oc", SPEECH_LANGUAGE_TR("Occitan"), "Occitan"},
  {"ka", SPEECH_LANGUAGE_TR("Georgian"), "ქართული"},
  {"be", SPEECH_LANGUAGE_TR("Belarusian"), "Беларуская"},
  {"tg", SPEECH_LANGUAGE_TR("Tajik"), "Тоҷикӣ"},
  {"sd", SPEECH_LANGUAGE_TR("Sindhi"), "سنڌي"},
  {"gu", SPEECH_LANGUAGE_TR("Gujarati"), "ગુજરાતી"},
  {"am", SPEECH_LANGUAGE_TR("Amharic"), "አማርኛ"},
  {"yi", SPEECH_LANGUAGE_TR("Yiddish"), "ייִדיש"},
  {"lo", SPEECH_LANGUAGE_TR("Lao"), "ລາວ"},
  {"uz", SPEECH_LANGUAGE_TR("Uzbek"), "Oʻzbek"},
  {"fo", SPEECH_LANGUAGE_TR("Faroese"), "Føroyskt"},
  {"ht", SPEECH_LANGUAGE_TR("Haitian Creole"), "Kreyòl ayisyen"},
  {"ps", SPEECH_LANGUAGE_TR("Pashto"), "پښتو"},
  {"tk", SPEECH_LANGUAGE_TR("Turkmen"), "Türkmen"},
  {"nn", SPEECH_LANGUAGE_TR("Norwegian Nynorsk"), "Nynorsk"},
  {"mt", SPEECH_LANGUAGE_TR("Maltese"), "Malti"},
  {"sa", SPEECH_LANGUAGE_TR("Sanskrit"), "संस्कृतम्"},
  {"lb", SPEECH_LANGUAGE_TR("Luxembourgish"), "Lëtzebuergesch"},
  {"my", SPEECH_LANGUAGE_TR("Burmese"), "မြန်မာ"},
  {"bo", SPEECH_LANGUAGE_TR("Tibetan"), "བོད་སྐད་"},
  {"tl", SPEECH_LANGUAGE_TR("Tagalog"), "Tagalog"},
  {"mg", SPEECH_LANGUAGE_TR("Malagasy"), "Malagasy"},
  {"as", SPEECH_LANGUAGE_TR("Assamese"), "অসমীয়া"},
  {"tt", SPEECH_LANGUAGE_TR("Tatar"), "Татарча"},
  {"haw", SPEECH_LANGUAGE_TR("Hawaiian"), "ʻŌlelo Hawaiʻi"},
  {"ln", SPEECH_LANGUAGE_TR("Lingala"), "Lingála"},
  {"ha", SPEECH_LANGUAGE_TR("Hausa"), "Hausa"},
  {"ba", SPEECH_LANGUAGE_TR("Bashkir"), "Башҡортса"},
  {"jw", SPEECH_LANGUAGE_TR("Javanese"), "Basa Jawa"},
  {"su", SPEECH_LANGUAGE_TR("Sundanese"), "Basa Sunda"},
  {"yue", SPEECH_LANGUAGE_TR("Cantonese"), "粵語"},
});
// clang-format on

constexpr std::span<const SpeechLanguage> entries() { return ENTRIES; }

constexpr const SpeechLanguage *find(std::string_view code) {
  auto it = std::ranges::find(ENTRIES, code, &SpeechLanguage::code);
  return it == ENTRIES.end() ? nullptr : &*it;
}

constexpr bool hasUniqueCodes() {
  for (std::size_t i = 0; i < ENTRIES.size(); ++i) {
    for (std::size_t j = i + 1; j < ENTRIES.size(); ++j) {
      if (ENTRIES[i].code == ENTRIES[j].code) return false;
    }
  }
  return true;
}

static_assert(hasUniqueCodes(), "speech language codes must be unique");

inline QString translatedName(const SpeechLanguage &lang) {
  return QCoreApplication::translate(TRANSLATION_CONTEXT, lang.name);
}

inline QString displayName(const SpeechLanguage &lang) {
  auto name = translatedName(lang);
  if (lang.nativeName == lang.name) return name;
  return QStringLiteral("%1 (%2)").arg(
      name, QString::fromUtf8(lang.nativeName.data(), static_cast<qsizetype>(lang.nativeName.size())));
}

inline const SpeechLanguage *systemLanguage() {
  auto code = QLocale::system().name().section(QLatin1Char('_'), 0, 0).toStdString();
  if (code == "nb") code = "no";
  if (code == "jv") code = "jw";
  return find(code);
}

} // namespace SpeechLanguageCatalogue
