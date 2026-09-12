#pragma once
#include <algorithm>
#include <array>
#include <span>
#include <string>
#include <string_view>
#include <QCoreApplication>
#include <QLocale>
#include <QString>
#include "services/local-speech-model-registry/speech-model-catalogue.hpp"

struct SpeechLanguage {
  std::string_view code;
  const char *name;
  std::string_view nativeName;
};

namespace SpeechLanguageCatalogue {

// clang-format off
constexpr auto ENTRIES = std::to_array<SpeechLanguage>({
  {"en", SPEECH_MODEL_TR("English"), "English"},
  {"zh", SPEECH_MODEL_TR("Chinese"), "中文"},
  {"de", SPEECH_MODEL_TR("German"), "Deutsch"},
  {"es", SPEECH_MODEL_TR("Spanish"), "Español"},
  {"ru", SPEECH_MODEL_TR("Russian"), "Русский"},
  {"ko", SPEECH_MODEL_TR("Korean"), "한국어"},
  {"fr", SPEECH_MODEL_TR("French"), "Français"},
  {"ja", SPEECH_MODEL_TR("Japanese"), "日本語"},
  {"pt", SPEECH_MODEL_TR("Portuguese"), "Português"},
  {"tr", SPEECH_MODEL_TR("Turkish"), "Türkçe"},
  {"pl", SPEECH_MODEL_TR("Polish"), "Polski"},
  {"ca", SPEECH_MODEL_TR("Catalan"), "Català"},
  {"nl", SPEECH_MODEL_TR("Dutch"), "Nederlands"},
  {"ar", SPEECH_MODEL_TR("Arabic"), "العربية"},
  {"sv", SPEECH_MODEL_TR("Swedish"), "Svenska"},
  {"it", SPEECH_MODEL_TR("Italian"), "Italiano"},
  {"id", SPEECH_MODEL_TR("Indonesian"), "Bahasa Indonesia"},
  {"hi", SPEECH_MODEL_TR("Hindi"), "हिन्दी"},
  {"fi", SPEECH_MODEL_TR("Finnish"), "Suomi"},
  {"vi", SPEECH_MODEL_TR("Vietnamese"), "Tiếng Việt"},
  {"he", SPEECH_MODEL_TR("Hebrew"), "עברית"},
  {"uk", SPEECH_MODEL_TR("Ukrainian"), "Українська"},
  {"el", SPEECH_MODEL_TR("Greek"), "Ελληνικά"},
  {"ms", SPEECH_MODEL_TR("Malay"), "Bahasa Melayu"},
  {"cs", SPEECH_MODEL_TR("Czech"), "Čeština"},
  {"ro", SPEECH_MODEL_TR("Romanian"), "Română"},
  {"da", SPEECH_MODEL_TR("Danish"), "Dansk"},
  {"hu", SPEECH_MODEL_TR("Hungarian"), "Magyar"},
  {"ta", SPEECH_MODEL_TR("Tamil"), "தமிழ்"},
  {"no", SPEECH_MODEL_TR("Norwegian"), "Norsk"},
  {"th", SPEECH_MODEL_TR("Thai"), "ไทย"},
  {"ur", SPEECH_MODEL_TR("Urdu"), "اردو"},
  {"hr", SPEECH_MODEL_TR("Croatian"), "Hrvatski"},
  {"bg", SPEECH_MODEL_TR("Bulgarian"), "Български"},
  {"lt", SPEECH_MODEL_TR("Lithuanian"), "Lietuvių"},
  {"la", SPEECH_MODEL_TR("Latin"), "Latina"},
  {"mi", SPEECH_MODEL_TR("Maori"), "Te Reo Māori"},
  {"ml", SPEECH_MODEL_TR("Malayalam"), "മലയാളം"},
  {"cy", SPEECH_MODEL_TR("Welsh"), "Cymraeg"},
  {"sk", SPEECH_MODEL_TR("Slovak"), "Slovenčina"},
  {"te", SPEECH_MODEL_TR("Telugu"), "తెలుగు"},
  {"fa", SPEECH_MODEL_TR("Persian"), "فارسی"},
  {"lv", SPEECH_MODEL_TR("Latvian"), "Latviešu"},
  {"bn", SPEECH_MODEL_TR("Bengali"), "বাংলা"},
  {"sr", SPEECH_MODEL_TR("Serbian"), "Српски"},
  {"az", SPEECH_MODEL_TR("Azerbaijani"), "Azərbaycan"},
  {"sl", SPEECH_MODEL_TR("Slovenian"), "Slovenščina"},
  {"kn", SPEECH_MODEL_TR("Kannada"), "ಕನ್ನಡ"},
  {"et", SPEECH_MODEL_TR("Estonian"), "Eesti"},
  {"mk", SPEECH_MODEL_TR("Macedonian"), "Македонски"},
  {"br", SPEECH_MODEL_TR("Breton"), "Brezhoneg"},
  {"eu", SPEECH_MODEL_TR("Basque"), "Euskara"},
  {"is", SPEECH_MODEL_TR("Icelandic"), "Íslenska"},
  {"hy", SPEECH_MODEL_TR("Armenian"), "Հայերեն"},
  {"ne", SPEECH_MODEL_TR("Nepali"), "नेपाली"},
  {"mn", SPEECH_MODEL_TR("Mongolian"), "Монгол"},
  {"bs", SPEECH_MODEL_TR("Bosnian"), "Bosanski"},
  {"kk", SPEECH_MODEL_TR("Kazakh"), "Қазақша"},
  {"sq", SPEECH_MODEL_TR("Albanian"), "Shqip"},
  {"sw", SPEECH_MODEL_TR("Swahili"), "Kiswahili"},
  {"gl", SPEECH_MODEL_TR("Galician"), "Galego"},
  {"mr", SPEECH_MODEL_TR("Marathi"), "मराठी"},
  {"pa", SPEECH_MODEL_TR("Punjabi"), "ਪੰਜਾਬੀ"},
  {"si", SPEECH_MODEL_TR("Sinhala"), "සිංහල"},
  {"km", SPEECH_MODEL_TR("Khmer"), "ខ្មែរ"},
  {"sn", SPEECH_MODEL_TR("Shona"), "chiShona"},
  {"yo", SPEECH_MODEL_TR("Yoruba"), "Yorùbá"},
  {"so", SPEECH_MODEL_TR("Somali"), "Soomaali"},
  {"af", SPEECH_MODEL_TR("Afrikaans"), "Afrikaans"},
  {"oc", SPEECH_MODEL_TR("Occitan"), "Occitan"},
  {"ka", SPEECH_MODEL_TR("Georgian"), "ქართული"},
  {"be", SPEECH_MODEL_TR("Belarusian"), "Беларуская"},
  {"tg", SPEECH_MODEL_TR("Tajik"), "Тоҷикӣ"},
  {"sd", SPEECH_MODEL_TR("Sindhi"), "سنڌي"},
  {"gu", SPEECH_MODEL_TR("Gujarati"), "ગુજરાતી"},
  {"am", SPEECH_MODEL_TR("Amharic"), "አማርኛ"},
  {"yi", SPEECH_MODEL_TR("Yiddish"), "ייִדיש"},
  {"lo", SPEECH_MODEL_TR("Lao"), "ລາວ"},
  {"uz", SPEECH_MODEL_TR("Uzbek"), "Oʻzbek"},
  {"fo", SPEECH_MODEL_TR("Faroese"), "Føroyskt"},
  {"ht", SPEECH_MODEL_TR("Haitian Creole"), "Kreyòl ayisyen"},
  {"ps", SPEECH_MODEL_TR("Pashto"), "پښتو"},
  {"tk", SPEECH_MODEL_TR("Turkmen"), "Türkmen"},
  {"nn", SPEECH_MODEL_TR("Norwegian Nynorsk"), "Nynorsk"},
  {"mt", SPEECH_MODEL_TR("Maltese"), "Malti"},
  {"sa", SPEECH_MODEL_TR("Sanskrit"), "संस्कृतम्"},
  {"lb", SPEECH_MODEL_TR("Luxembourgish"), "Lëtzebuergesch"},
  {"my", SPEECH_MODEL_TR("Burmese"), "မြန်မာ"},
  {"bo", SPEECH_MODEL_TR("Tibetan"), "བོད་སྐད་"},
  {"tl", SPEECH_MODEL_TR("Tagalog"), "Tagalog"},
  {"mg", SPEECH_MODEL_TR("Malagasy"), "Malagasy"},
  {"as", SPEECH_MODEL_TR("Assamese"), "অসমীয়া"},
  {"tt", SPEECH_MODEL_TR("Tatar"), "Татарча"},
  {"haw", SPEECH_MODEL_TR("Hawaiian"), "ʻŌlelo Hawaiʻi"},
  {"ln", SPEECH_MODEL_TR("Lingala"), "Lingála"},
  {"ha", SPEECH_MODEL_TR("Hausa"), "Hausa"},
  {"ba", SPEECH_MODEL_TR("Bashkir"), "Башҡортса"},
  {"jw", SPEECH_MODEL_TR("Javanese"), "Basa Jawa"},
  {"su", SPEECH_MODEL_TR("Sundanese"), "Basa Sunda"},
  {"yue", SPEECH_MODEL_TR("Cantonese"), "粵語"},
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
  return QCoreApplication::translate(SpeechModelCatalogue::TRANSLATION_CONTEXT, lang.name);
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
