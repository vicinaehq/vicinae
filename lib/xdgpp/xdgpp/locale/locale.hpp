#pragma once
#include <optional>
#include <ostream>
#include <string>
#include <string_view>

namespace xdgpp {
class Locale {
public:
  /**
   * Returns a suitable system locale.
   */
  static Locale system();
  static Locale parse(std::string_view data);

  Locale();
  Locale(std::string_view data);

  const std::string &lang() const { return m_lang; }
  const std::optional<std::string> &country() const { return m_country; }
  const std::optional<std::string> &encoding() const { return m_encoding; }
  const std::optional<std::string> &modifier() const { return m_modifier; }

  void setLang(const std::string &lang) { m_lang = lang; }
  void setCountry(const std::optional<std::string> &country) { m_country = country; }
  void setEncoding(const std::optional<std::string> &encoding) { m_encoding = encoding; }
  void setModifier(const std::optional<std::string> &mod) { m_modifier = mod; }

  std::string toString() const;

private:
  std::string m_lang;
  std::optional<std::string> m_country;
  std::optional<std::string> m_encoding;
  std::optional<std::string> m_modifier;
};
}; // namespace xdgpp

std::ostream &operator<<(std::ostream &ofs, const xdgpp::Locale &locale);
