#include "locale.hpp"
#include <ostream>

namespace xdgpp {

Locale Locale::system() {
  setlocale(LC_MESSAGES, "");
  const char *messagesLocale = setlocale(LC_MESSAGES, nullptr);

  return Locale::parse(messagesLocale);
}

bool Locale::matchesOnly(const Locale &rhs, int components) {
  if (rhs.flags() != components) return false;
  if (components & Component::LANG && m_lang != rhs.lang()) return false;
  if (components & Component::COUNTRY && m_country != rhs.country()) return false;
  if (components & Component::MODIFIER && m_modifier != rhs.modifier()) return false;
  return true;
}

int Locale::flags() const {
  int value = static_cast<int>(Component::LANG);
  if (m_country) value |= Component::COUNTRY;
  if (m_modifier) value |= Component::MODIFIER;
  return value;
}

Locale Locale::parse(std::string_view data) {
  enum class State { Lang, Country, Encoding, Mod } state = State::Lang;
  size_t i = 0;
  std::string lang;
  std::string country;
  std::string encoding;
  std::string mod;

  while (i < data.size() && data[i] != ']') {
    const char c = data[i];

    switch (state) {
    case State::Lang: {
      if (std::isalpha(c)) {
        lang += c;
      } else {
        if (c == '_') {
          state = State::Country;
        } else if (c == '.') {
          state = State::Encoding;
        } else if (c == '@') {
          state = State::Mod;
        }
      }
      break;
    }
    case State::Country: {
      if (std::isalpha(c)) {
        country += c;
      } else {
        if (c == '.') {
          state = State::Encoding;
        } else if (c == '@') {
          state = State::Mod;
        }
      }
      break;
    }
    case State::Encoding: {
      if (std::isalnum(c)) {
        encoding += c;
      } else {
        if (c == '@') { state = State::Mod; }
      }
      break;
    }
    case State::Mod: {
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

Locale::Locale() = default;

Locale::Locale(std::string_view data) { *this = Locale::parse(data); }

std::string Locale::toString() const {
  std::string s = lang();

  if (country()) {
    s += '_';
    s.append(*country());
  }

  if (encoding()) {
    s += '.';
    s.append(*encoding());
  }

  if (modifier()) {
    s += '@';
    s.append(*modifier());
  }

  return s;
}

std::ostream &operator<<(std::ostream &ofs, const xdgpp::Locale &locale) {
  ofs << locale.toString();
  return ofs;
}

}; // namespace xdgpp
