#include "locale.hpp"
#include <ostream>

namespace xdgpp {

Locale Locale::system() {
  setlocale(LC_MESSAGES, "");
  char *messagesLocale = setlocale(LC_MESSAGES, nullptr);

  return Locale::parse(messagesLocale);
}

Locale Locale::parse(std::string_view data) {
  enum State { Lang, Country, Encoding, Mod } state = Lang;
  size_t i = 0;
  std::string lang;
  std::string country;
  std::string encoding;
  std::string mod;

  while (i < data.size() && data[i] != ']') {
    char c = data[i];

    switch (state) {
    case Lang: {
      if (std::isalpha(c)) {
        lang += c;
      } else {
        if (c == '_') {
          state = Country;
        } else if (c == '.') {
          state = Encoding;
        } else if (c == '@') {
          state = Mod;
        }
      }
      break;
    }
    case Country: {
      if (std::isalpha(c)) {
        country += c;
      } else {
        if (c == '.') {
          state = Encoding;
        } else if (c == '@') {
          state = Mod;
        }
      }
      break;
    }
    case Encoding: {
      if (std::isalnum(c)) {
        encoding += c;
      } else {
        if (c == '@') { state = Mod; }
      }
      break;
    }
    case Mod: {
      if (std::isalpha(c)) {
        mod += c;
      } else {
      }
      break;
    }
    }
    ++i;
  }

  Locale locale;

  locale.setLang(lang);
  if (!country.empty()) locale.setCountry(country);
  if (!encoding.empty()) locale.setEncoding(encoding);
  if (!mod.empty()) locale.setModifier(mod);

  return locale;
}

Locale::Locale() {}

Locale::Locale(std::string_view data) { *this = Locale::parse(data); }

std::string Locale::toString() const {
  std::string s = lang();

  if (country()) s = s + '_' + *country();

  if (encoding()) s = s + '.' + *encoding();

  if (modifier()) s = s + '@' + *modifier();

  return s;
}

std::ostream &operator<<(std::ostream &ofs, const xdgpp::Locale &locale) {
  ofs << locale.toString();
  return ofs;
}

}; // namespace xdgpp
