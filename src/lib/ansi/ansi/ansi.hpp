#pragma once

#include <array>
#include <cstdint>
#include <cstdio>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace ansi {

struct Color {
  enum class Type : uint8_t { Standard, Bright, Palette256, TrueColor };

  Type type;
  uint8_t index = 0;
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;

  static constexpr auto standard(uint8_t i) -> Color { return {Type::Standard, i}; }
  static constexpr auto bright(uint8_t i) -> Color { return {Type::Bright, i}; }
  static constexpr auto palette(uint8_t i) -> Color { return {Type::Palette256, i}; }
  static constexpr auto rgb(uint8_t r, uint8_t g, uint8_t b) -> Color {
    return {Type::TrueColor, 0, r, g, b};
  }

  auto operator==(const Color &) const -> bool = default;
};

struct Style {
  std::optional<Color> fg;
  std::optional<Color> bg;
  bool bold = false;
  bool dim = false;
  bool italic = false;
  bool underline = false;
  bool strikethrough = false;
  bool inverse = false;
  bool hidden = false;
  bool overline = false;

  auto operator==(const Style &) const -> bool = default;

  [[nodiscard]] constexpr auto is_default() const -> bool {
    return !fg && !bg && !bold && !dim && !italic && !underline && !strikethrough && !inverse && !hidden &&
           !overline;
  }
};

struct Segment {
  std::string_view text;
  Style style;
};

// Splits an ANSI-colored string into styled text segments.
class Tokenizer {
public:
  explicit Tokenizer(std::string_view input) : m_input(input) {}

  auto next() -> std::optional<Segment> {
    consume_escapes();
    if (m_pos >= m_input.size()) return {};

    Style style = m_style;
    size_t start = m_pos;

    while (m_pos < m_input.size() && m_input[m_pos] != '\x1b') {
      ++m_pos;
    }

    return Segment{m_input.substr(start, m_pos - start), style};
  }

private:
  void consume_escapes() {
    while (m_pos < m_input.size() && m_input[m_pos] == '\x1b') {
      ++m_pos;
      if (m_pos >= m_input.size()) return;

      if (m_input[m_pos] != '[') {
        ++m_pos;
        continue;
      }
      ++m_pos;

      std::vector<int> params;
      params.reserve(8);
      params.emplace_back(0);
      bool skip = false;

      while (m_pos < m_input.size()) {
        char c = m_input[m_pos];

        if (c >= '0' && c <= '9') {
          if (!skip) params.back() = params.back() * 10 + (c - '0');
        } else if (c == ';') {
          if (!skip) params.emplace_back(0);
        } else if (c >= 0x40 && c <= 0x7E) {
          ++m_pos;
          if (!skip && c == 'm') apply_sgr(params);
          break;
        } else {
          skip = true;
        }

        ++m_pos;
      }
    }
  }

  void apply_sgr(const std::vector<int> &params) {
    for (size_t i = 0; i < params.size(); ++i) {
      int code = params[i];

      switch (code) {
      case 0:
        m_style = {};
        break;
      case 1:
        m_style.bold = true;
        break;
      case 2:
        m_style.dim = true;
        break;
      case 3:
        m_style.italic = true;
        break;
      case 4:
        m_style.underline = true;
        break;
      case 7:
        m_style.inverse = true;
        break;
      case 8:
        m_style.hidden = true;
        break;
      case 9:
        m_style.strikethrough = true;
        break;
      case 21:
        m_style.underline = true;
        break;
      case 22:
        m_style.bold = false;
        m_style.dim = false;
        break;
      case 23:
        m_style.italic = false;
        break;
      case 24:
        m_style.underline = false;
        break;
      case 27:
        m_style.inverse = false;
        break;
      case 28:
        m_style.hidden = false;
        break;
      case 29:
        m_style.strikethrough = false;
        break;
      case 39:
        m_style.fg.reset();
        break;
      case 49:
        m_style.bg.reset();
        break;
      case 53:
        m_style.overline = true;
        break;
      case 55:
        m_style.overline = false;
        break;
      default:
        if (code >= 30 && code <= 37) {
          m_style.fg = Color::standard(code - 30);
        } else if (code >= 40 && code <= 47) {
          m_style.bg = Color::standard(code - 40);
        } else if (code >= 90 && code <= 97) {
          m_style.fg = Color::bright(code - 90);
        } else if (code >= 100 && code <= 107) {
          m_style.bg = Color::bright(code - 100);
        } else if (code == 38 || code == 48) {
          parse_extended_color(params, i, code == 38);
        }
        break;
      }
    }
  }

  void parse_extended_color(const std::vector<int> &params, size_t &i, bool foreground) {
    if (i + 1 >= params.size()) return;
    int mode = params[i + 1];

    if (mode == 5 && i + 2 < params.size()) {
      auto &target = foreground ? m_style.fg : m_style.bg;
      target = Color::palette(static_cast<uint8_t>(params[i + 2]));
      i += 2;
    } else if (mode == 2 && i + 4 < params.size()) {
      auto &target = foreground ? m_style.fg : m_style.bg;
      target = Color::rgb(static_cast<uint8_t>(params[i + 2]), static_cast<uint8_t>(params[i + 3]),
                          static_cast<uint8_t>(params[i + 4]));
      i += 4;
    }
  }

  std::string_view m_input;
  size_t m_pos = 0;
  Style m_style;
};

// Remove all ANSI escape sequences, returning plain text.
inline auto strip(std::string_view input) -> std::string {
  std::string out;
  out.reserve(input.size());
  Tokenizer tok(input);
  while (auto seg = tok.next()) {
    out.append(seg->text);
  }
  return out;
}

// Maps standard/bright color indices (0-7) to CSS hex strings.
// Uses std::string so callers can populate from runtime-resolved theme colors.
struct Palette {
  std::array<std::string, 8> standard = {"#2e3436", "#cc0000", "#4e9a06", "#c4a000",
                                         "#3465a4", "#75507b", "#06989a", "#d3d7cf"};
  std::array<std::string, 8> bright = {"#555753", "#ef2929", "#8ae234", "#fce94f",
                                       "#729fcf", "#ad7fa8", "#34e2e2", "#eeeeec"};
};

// Turn a Color into a CSS hex string using the given palette.
inline auto resolve_color(const Color &c, const Palette &pal) -> std::string {
  switch (c.type) {
  case Color::Type::Standard:
    return std::string(pal.standard[c.index % 8]);
  case Color::Type::Bright:
    return std::string(pal.bright[c.index % 8]);
  case Color::Type::Palette256: {
    if (c.index < 8) return std::string(pal.standard[c.index]);
    if (c.index < 16) return std::string(pal.bright[c.index - 8]);

    if (c.index < 232) {
      int idx = c.index - 16;
      int ri = idx / 36;
      int gi = (idx % 36) / 6;
      int bi = idx % 6;
      auto map = [](int v) { return v == 0 ? 0 : 55 + v * 40; };
      char buf[8];
      std::snprintf(buf, sizeof(buf), "#%02x%02x%02x", map(ri), map(gi), map(bi));
      return buf;
    }

    int gray = 8 + (c.index - 232) * 10;
    char buf[8];
    std::snprintf(buf, sizeof(buf), "#%02x%02x%02x", gray, gray, gray);
    return buf;
  }
  case Color::Type::TrueColor: {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "#%02x%02x%02x", c.r, c.g, c.b);
    return buf;
  }
  }
  return {};
}

// Convert an ANSI-colored string to an HTML fragment with <span> styling.
// Text content is HTML-escaped. No outer wrapper element is added.
inline auto to_html(std::string_view input, const Palette &pal = {}) -> std::string {
  std::string html;
  html.reserve(input.size() * 2);

  bool in_span = false;
  Style prev;

  Tokenizer tok(input);
  while (auto seg = tok.next()) {
    if (!(seg->style == prev)) {
      if (in_span) {
        html += "</span>";
        in_span = false;
      }

      if (!seg->style.is_default()) {
        html += "<span style=\"";
        bool need_sep = false;

        auto append_prop = [&](std::string_view prop) {
          if (need_sep) html += ';';
          html += prop;
          need_sep = true;
        };

        const Style &s = seg->style;

        if (s.fg) append_prop("color:" + resolve_color(*s.fg, pal));
        if (s.bg) append_prop("background:" + resolve_color(*s.bg, pal));
        if (s.bold) append_prop("font-weight:bold");
        if (s.dim) append_prop("opacity:0.5");
        if (s.italic) append_prop("font-style:italic");
        if (s.hidden) append_prop("visibility:hidden");

        {
          std::string deco;
          if (s.underline) deco += "underline";
          if (s.strikethrough) {
            if (!deco.empty()) deco += ' ';
            deco += "line-through";
          }
          if (s.overline) {
            if (!deco.empty()) deco += ' ';
            deco += "overline";
          }
          if (!deco.empty()) append_prop("text-decoration:" + deco);
        }

        html += "\">";
        in_span = true;
      }

      prev = seg->style;
    }

    for (char c : seg->text) {
      switch (c) {
      case '&':
        html += "&amp;";
        break;
      case '<':
        html += "&lt;";
        break;
      case '>':
        html += "&gt;";
        break;
      case '"':
        html += "&quot;";
        break;
      default:
        html += c;
        break;
      }
    }
  }

  if (in_span) html += "</span>";
  return html;
}

} // namespace ansi
