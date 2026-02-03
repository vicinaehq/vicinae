#include "reader.hpp"
#include <iostream>

namespace xdgpp {

void DesktopEntryReader::parseGroupHeader() {
  std::string name;

  ++m_cursor;

  while (m_cursor < m_data.size() && isGroupHeaderChar(m_data[m_cursor])) {
    name += m_data[m_cursor];
    ++m_cursor;
  }

  if (m_cursor == m_data.size() || m_data[m_cursor] != ']') {
    warnings.emplace_back(std::string("Expected ] to finish group header name but got ") + m_data[m_cursor]);
  }

  auto grp = std::make_unique<Group>(name);

  m_currentGroup = grp.get();
  m_groups[grp->name()] = std::move(grp);

  ++m_cursor;
}

std::string DesktopEntryReader::parseKey() {
  std::string key;
  while (m_cursor < m_data.size() && isKeyChar(m_data[m_cursor])) {
    key += m_data[m_cursor];
    ++m_cursor;
  }

  return key;
}

std::string DesktopEntryReader::parseRawValue() {
  std::string value;
  while (peek() && peek() != LF) {
    value += consume();
  }
  size_t trimmable = 0;
  for (auto it = value.rbegin(); it != value.rend() && std::isspace(*it); ++it) {
    ++trimmable;
  }

  return value.substr(0, value.size() - trimmable);
}

void DesktopEntryReader::skipSpace() {
  while (isInlineSpace(peek())) {
    consume();
  }
}

void DesktopEntryReader::consume(char c) {
  if (peek() == c) consume();
}

char DesktopEntryReader::consume() {
  char c = peek();
  ++m_cursor;
  return c;
}

char DesktopEntryReader::peek() const {
  if (m_cursor < m_data.size()) return m_data[m_cursor];
  return 0;
}

bool DesktopEntryReader::isPeek(char c) const { return peek() && peek() == c; }

std::string DesktopEntryReader::parseRawLocale() {
  std::string locale;

  consume('[');
  while (!isPeek(']')) {
    locale += consume();
  }
  consume(']');

  return locale;
}

size_t DesktopEntryReader::computeLocalScore(const Locale &locale) {
  using F = Locale::Component;

  if (m_locale.exactFlags(F::LANG | F::COUNTRY | F::MODIFIER)) {
    if (m_locale.matchesOnly(locale, F::LANG | F::COUNTRY | F::MODIFIER)) return 4;
    if (m_locale.matchesOnly(locale, F::LANG || Locale::COUNTRY)) return 3;
    if (m_locale.matchesOnly(locale, F::LANG | Locale::MODIFIER)) return 2;
    if (m_locale.matchesOnly(locale, F::LANG)) return 1;
    return 0;
  }

  if (m_locale.exactFlags(F::LANG | F::COUNTRY)) {
    if (m_locale.matchesOnly(locale, F::LANG | F::COUNTRY)) return 2;
    if (m_locale.matchesOnly(locale, F::LANG)) return 1;
    return 0;
  }

  if (m_locale.exactFlags(F::LANG | F::MODIFIER)) {
    if (m_locale.matchesOnly(locale, F::LANG | F::MODIFIER)) return 2;
    if (m_locale.matchesOnly(locale, F::LANG)) return 1;
    return 0;
  }

  if (m_locale.exactFlags(F::LANG)) {
    if (m_locale.matchesOnly(locale, F::LANG)) return 1;
    return 0;
  }

  return 0;
}

void DesktopEntryReader::parse() {
  enum State { Reset, Comment } state = Reset;

  while (peek()) {
    char c = peek();

    switch (state) {
    case Reset: {
      if (c == '#') {
        state = Comment;
      } else if (c == '[') {
        parseGroupHeader();
        state = Reset;
        continue;
      } else if (isKeyChar(c)) {
        parseEntry();
        state = Reset;
        continue;
      }
    }
    case Comment: {
      if (c == LF) { state = Reset; }
    }
    }
    consume();
  }
}

void DesktopEntryReader::parseEntry() {
  std::string key = parseKey();
  std::optional<Locale> locale;

  skipSpace();
  if (peek() == '[') { locale = Locale::parse(parseRawLocale()); }

  // if we don't get expected '=' separator we just skip the current line.
  if (consume() != '=') {
    while (peek() && peek() != LF) {
      consume();
    }
    return;
  }

  skipSpace();
  std::string value = parseRawValue();

  if (!m_currentGroup) return;

  if (locale) {
    size_t score = computeLocalScore(locale.value());

    if (!score) return;

    if (auto current = m_currentGroup->localizedKey(key)) {
      if (computeLocalScore(current->locale) > score) return;
    }

    m_currentGroup->setLocalized(key, value, locale.value());
    return;
  }

  m_currentGroup->set(key, value);
}
}; // namespace xdgpp
