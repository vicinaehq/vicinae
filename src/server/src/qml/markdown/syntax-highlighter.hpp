#pragma once

#include "theme/colors.hpp"
#include "theme/theme-file.hpp"
#include <KSyntaxHighlighting/AbstractHighlighter>
#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/Format>
#include <KSyntaxHighlighting/Repository>
#include <KSyntaxHighlighting/State>
#include <KSyntaxHighlighting/Theme>
#include <QString>
#include <QStringView>
#include <array>

namespace syntax {

inline KSyntaxHighlighting::Repository &repository() {
  static KSyntaxHighlighting::Repository repo;
  return repo;
}

struct StyleInfo {
  QString color;
  bool bold = false;
  bool italic = false;
};

using TextStyle = KSyntaxHighlighting::Theme::TextStyle;
using StyleMap = std::array<StyleInfo, static_cast<int>(TextStyle::Others) + 1>;

inline StyleMap buildStyleMap(const ThemeFile &theme) {
  auto hex = [&](SemanticColor c) {
    return theme.resolve(c).name(QColor::HexRgb);
  };

  StyleMap map{};

  auto set = [&](TextStyle ts, SemanticColor c, bool bold = false, bool italic = false) {
    auto i = static_cast<int>(ts);
    map[i] = {hex(c), bold, italic};
  };

  set(TextStyle::Keyword,        SemanticColor::Purple,  true);
  set(TextStyle::ControlFlow,    SemanticColor::Purple,  true);
  set(TextStyle::Function,       SemanticColor::Blue);
  set(TextStyle::Variable,       SemanticColor::Red);
  set(TextStyle::Operator,       SemanticColor::Foreground);
  set(TextStyle::BuiltIn,        SemanticColor::Cyan);
  set(TextStyle::Extension,      SemanticColor::Cyan);
  set(TextStyle::Preprocessor,   SemanticColor::Orange);
  set(TextStyle::Attribute,      SemanticColor::Purple);
  set(TextStyle::Char,           SemanticColor::Green);
  set(TextStyle::SpecialChar,    SemanticColor::Orange);
  set(TextStyle::String,         SemanticColor::Green);
  set(TextStyle::VerbatimString, SemanticColor::Green);
  set(TextStyle::SpecialString,  SemanticColor::Green);
  set(TextStyle::Import,         SemanticColor::Purple);
  set(TextStyle::DataType,       SemanticColor::Cyan);
  set(TextStyle::DecVal,         SemanticColor::Orange);
  set(TextStyle::BaseN,          SemanticColor::Orange);
  set(TextStyle::Float,          SemanticColor::Orange);
  set(TextStyle::Constant,       SemanticColor::Orange);
  set(TextStyle::Comment,        SemanticColor::TextMuted, false, true);
  set(TextStyle::Documentation,  SemanticColor::TextMuted, false, true);
  set(TextStyle::Annotation,     SemanticColor::Green,     false, true);
  set(TextStyle::CommentVar,     SemanticColor::TextMuted, false, true);
  set(TextStyle::RegionMarker,   SemanticColor::TextMuted);
  set(TextStyle::Information,    SemanticColor::Blue);
  set(TextStyle::Warning,        SemanticColor::Orange);
  set(TextStyle::Alert,          SemanticColor::Red);
  set(TextStyle::Error,          SemanticColor::Red);

  return map;
}

class HtmlHighlighter : public KSyntaxHighlighting::AbstractHighlighter {
public:
  void setStyleMap(const StyleMap &map) { m_styles = &map; }

  QString result() const { return m_result; }

  void highlightCode(QStringView code) {
    m_result.clear();
    KSyntaxHighlighting::State state;
    const auto lines = code.split(u'\n');
    for (int i = 0; i < lines.size(); ++i) {
      m_currentLine = lines[i];
      m_lineResult.clear();
      m_lastOffset = 0;
      state = highlightLine(m_currentLine, state);
      flushPending();
      m_result += m_lineResult;
      if (i + 1 < lines.size())
        m_result += u'\n';
    }
  }

protected:
  void applyFormat(int offset, int length, const KSyntaxHighlighting::Format &format) override {
    if (offset > m_lastOffset)
      appendEscaped(m_currentLine.mid(m_lastOffset, offset - m_lastOffset));

    auto text = m_currentLine.mid(offset, length);
    if (!format.isValid() || !m_styles) {
      appendEscaped(text);
    } else {
      auto idx = static_cast<int>(format.textStyle());
      const auto &style = (*m_styles)[idx];
      bool hasStyle = !style.color.isEmpty() || style.bold || style.italic;

      if (hasStyle) {
        m_lineResult += QStringLiteral("<span style=\"");
        if (!style.color.isEmpty())
          m_lineResult += QStringLiteral("color:") + style.color + u';';
        if (style.bold)
          m_lineResult += QStringLiteral("font-weight:bold;");
        if (style.italic)
          m_lineResult += QStringLiteral("font-style:italic;");
        m_lineResult += QStringLiteral("\">");
        appendEscaped(text);
        m_lineResult += QStringLiteral("</span>");
      } else {
        appendEscaped(text);
      }
    }
    m_lastOffset = offset + length;
  }

private:
  void flushPending() {
    if (m_lastOffset < m_currentLine.size())
      appendEscaped(m_currentLine.mid(m_lastOffset));
  }

  void appendEscaped(QStringView text) {
    for (auto ch : text) {
      switch (ch.unicode()) {
      case u'&': m_lineResult += QStringLiteral("&amp;"); break;
      case u'<': m_lineResult += QStringLiteral("&lt;"); break;
      case u'>': m_lineResult += QStringLiteral("&gt;"); break;
      case u'"': m_lineResult += QStringLiteral("&quot;"); break;
      default: m_lineResult += ch; break;
      }
    }
  }

  const StyleMap *m_styles = nullptr;
  QStringView m_currentLine;
  QString m_lineResult;
  QString m_result;
  int m_lastOffset = 0;
};

inline QString highlight(const QString &code, const QString &language,
                         const StyleMap &styles, bool isDark) {
  auto wrapPre = [](const QString &inner) {
    return QStringLiteral("<pre>") + inner + QStringLiteral("</pre>");
  };

  if (language.isEmpty())
    return wrapPre(code.toHtmlEscaped());

  auto &repo = repository();
  auto def = repo.definitionForName(language);
  if (!def.isValid())
    def = repo.definitionForFileName(QStringLiteral("file.") + language);
  if (!def.isValid())
    return wrapPre(code.toHtmlEscaped());

  auto themeType = isDark ? KSyntaxHighlighting::Repository::DarkTheme
                          : KSyntaxHighlighting::Repository::LightTheme;

  HtmlHighlighter hl;
  hl.setDefinition(def);
  hl.setTheme(repo.defaultTheme(themeType));
  hl.setStyleMap(styles);
  hl.highlightCode(code);
  return wrapPre(hl.result());
}

} // namespace syntax
