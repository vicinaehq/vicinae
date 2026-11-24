#pragma once

#include <QString>
#include <QStringView>
#include <KSyntaxHighlighting/AbstractHighlighter>
#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/Format>
#include <KSyntaxHighlighting/Repository>
#include <KSyntaxHighlighting/State>
#include <KSyntaxHighlighting/Theme>
#include "syntax-palette.hpp"
#include "theme/colors.hpp"
#include "theme/theme-file.hpp"

namespace syntax {

inline KSyntaxHighlighting::Repository &repository() {
  static thread_local KSyntaxHighlighting::Repository repo;
  return repo;
}

using StyleMap = Vicinae::Syntax::StyleMap;

inline Vicinae::Syntax::Palette paletteForTheme(const ThemeFile &theme) {
  return {.background = theme.resolve(SemanticColor::Background),
          .foreground = theme.resolve(SemanticColor::Foreground),
          .keyword = theme.resolve(SemanticColor::SyntaxKeyword),
          .controlFlow = theme.resolve(SemanticColor::SyntaxControlFlow),
          .function = theme.resolve(SemanticColor::SyntaxFunction),
          .type = theme.resolve(SemanticColor::SyntaxType),
          .string = theme.resolve(SemanticColor::SyntaxString),
          .number = theme.resolve(SemanticColor::SyntaxNumber),
          .constant = theme.resolve(SemanticColor::SyntaxConstant),
          .attribute = theme.resolve(SemanticColor::SyntaxAttribute),
          .preprocessor = theme.resolve(SemanticColor::SyntaxPreprocessor),
          .special = theme.resolve(SemanticColor::SyntaxSpecial),
          .error = theme.resolve(SemanticColor::SyntaxError),
          .comment = theme.resolve(SemanticColor::SyntaxComment),
          .heading = theme.resolve(SemanticColor::SyntaxHeading),
          .emphasis = theme.resolve(SemanticColor::SyntaxEmphasis),
          .link = theme.resolve(SemanticColor::SyntaxLink),
          .linkTarget = theme.resolve(SemanticColor::SyntaxLinkTarget),
          .inlineCode = theme.resolve(SemanticColor::SyntaxInlineCode),
          .marker = theme.resolve(SemanticColor::SyntaxMarker)};
}

inline StyleMap buildStyleMap(const ThemeFile &theme) { return paletteForTheme(theme).styles(); }

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
      if (i + 1 < lines.size()) m_result += u'\n';
    }
  }

protected:
  void applyFormat(int offset, int length, const KSyntaxHighlighting::Format &format) override {
    if (offset > m_lastOffset) appendEscaped(m_currentLine.mid(m_lastOffset, offset - m_lastOffset));

    auto text = m_currentLine.mid(offset, length);
    if (!format.isValid() || !m_styles) {
      appendEscaped(text);
    } else {
      const auto &color = (*m_styles)[format.textStyle()];
      m_lineResult += QStringLiteral("<span style=\"color:") + color.name(QColor::HexRgb) + u';';
      if (format.isBold({})) m_lineResult += QStringLiteral("font-weight:bold;");
      if (format.isItalic({})) m_lineResult += QStringLiteral("font-style:italic;");
      if (format.isUnderline({})) m_lineResult += QStringLiteral("text-decoration:underline;");
      if (format.isStrikeThrough({})) m_lineResult += QStringLiteral("text-decoration:line-through;");
      m_lineResult += QStringLiteral("\">");
      appendEscaped(text);
      m_lineResult += QStringLiteral("</span>");
    }
    m_lastOffset = offset + length;
  }

private:
  void flushPending() {
    if (m_lastOffset < m_currentLine.size()) appendEscaped(m_currentLine.mid(m_lastOffset));
  }

  void appendEscaped(QStringView text) {
    for (auto ch : text) {
      switch (ch.unicode()) {
      case u'&':
        m_lineResult += QStringLiteral("&amp;");
        break;
      case u'<':
        m_lineResult += QStringLiteral("&lt;");
        break;
      case u'>':
        m_lineResult += QStringLiteral("&gt;");
        break;
      case u'"':
        m_lineResult += QStringLiteral("&quot;");
        break;
      default:
        m_lineResult += ch;
        break;
      }
    }
  }

  const StyleMap *m_styles = nullptr;
  QStringView m_currentLine;
  QString m_lineResult;
  QString m_result;
  int m_lastOffset = 0;
};

inline QString highlight(const QString &code, const QString &language, const StyleMap &styles, bool isDark,
                         const QString &monoFamily) {
  // Qt's HTML importer gives <pre> the platform fixed font (Courier New on Windows), overriding the
  // TextEdit font; an inline style wins over that default.
  auto wrapPre = [&monoFamily](const QString &inner) {
    return QStringLiteral("<pre style=\"font-family:'%1',monospace;\">").arg(monoFamily) + inner +
           QStringLiteral("</pre>");
  };

  if (language.isEmpty()) return wrapPre(code.toHtmlEscaped());

  auto &repo = repository();
  auto def = repo.definitionForName(language);
  if (!def.isValid()) def = repo.definitionForFileName(QStringLiteral("file.") + language);
  if (!def.isValid()) return wrapPre(code.toHtmlEscaped());

  auto themeType =
      isDark ? KSyntaxHighlighting::Repository::DarkTheme : KSyntaxHighlighting::Repository::LightTheme;

  HtmlHighlighter hl;
  hl.setDefinition(def);
  hl.setTheme(repo.defaultTheme(themeType));
  hl.setStyleMap(styles);
  hl.highlightCode(code);
  return wrapPre(hl.result());
}

} // namespace syntax
