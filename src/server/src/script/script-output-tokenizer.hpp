#pragma once
#include "theme.hpp"
#include <QColor>

/**
 * Tokenizer for script outputs, with support for basic ansi colors and URL recognition.
 */
class ScriptOutputTokenizer {
public:
  struct Format {
    bool underline = false;
    bool reset = false;
    bool italic = false;
    std::optional<QColor> fg;
    std::optional<QColor> bg;
  };

  struct Token {
    QString text;
    bool url = false;
    std::optional<Format> fmt;
  };

  ScriptOutputTokenizer(QStringView str);

  size_t cursor() const;
  void setCursor(size_t cursor);
  std::optional<Token> next();

private:
  enum State : std::uint8_t { Normal, Escape, Color, Url };
  static ColorLike getStandardColor(std::uint8_t code);
  static QColor parseFgColor(std::uint8_t code);
  static bool isValidUrlChar(QChar c);
  static QColor parseBgColor(int code);
  static Format parseColor(const std::vector<std::uint8_t> &codes);

  QStringView m_data;
  size_t m_cursor = 0;
  State m_state = Normal;
};
