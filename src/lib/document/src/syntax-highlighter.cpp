#include <QStringView>
#include <KSyntaxHighlighting/AbstractHighlighter>
#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/Format>
#include <KSyntaxHighlighting/Repository>
#include <KSyntaxHighlighting/State>
#include "syntax-highlighter.hpp"
#include "document-style.hpp"

namespace vicinae::document::syntax {

namespace {

KSyntaxHighlighting::Repository &repository() {
  static thread_local KSyntaxHighlighting::Repository repo;
  return repo;
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
      auto idx = static_cast<int>(format.textStyle());
      const auto &style = (*m_styles)[idx];
      bool hasStyle = !style.color.isEmpty() || style.bold || style.italic;

      if (hasStyle) {
        m_lineResult += QStringLiteral("<span style=\"");
        if (!style.color.isEmpty()) m_lineResult += QStringLiteral("color:") + style.color + u';';
        if (style.bold) m_lineResult += QStringLiteral("font-weight:bold;");
        if (style.italic) m_lineResult += QStringLiteral("font-style:italic;");
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

} // namespace

StyleMap buildStyleMap(const DocumentStyle &theme) {
  auto hex = [](const QColor &color) { return color.name(QColor::HexRgb); };

  StyleMap map{};

  auto set = [&](TextStyle ts, const QColor &c, bool bold = false, bool italic = false) {
    auto i = static_cast<int>(ts);
    map[i] = {hex(c), bold, italic};
  };

  set(TextStyle::Keyword, theme.keywordColor, true);
  set(TextStyle::ControlFlow, theme.keywordColor, true);
  set(TextStyle::Function, theme.functionColor);
  set(TextStyle::Variable, theme.variableColor);
  set(TextStyle::Operator, theme.foreground);
  set(TextStyle::BuiltIn, theme.builtinColor);
  set(TextStyle::Extension, theme.builtinColor);
  set(TextStyle::Preprocessor, theme.numberColor);
  set(TextStyle::Attribute, theme.keywordColor);
  set(TextStyle::Char, theme.stringColor);
  set(TextStyle::SpecialChar, theme.numberColor);
  set(TextStyle::String, theme.stringColor);
  set(TextStyle::VerbatimString, theme.stringColor);
  set(TextStyle::SpecialString, theme.stringColor);
  set(TextStyle::Import, theme.keywordColor);
  set(TextStyle::DataType, theme.builtinColor);
  set(TextStyle::DecVal, theme.numberColor);
  set(TextStyle::BaseN, theme.numberColor);
  set(TextStyle::Float, theme.numberColor);
  set(TextStyle::Constant, theme.numberColor);
  set(TextStyle::Comment, theme.commentColor, false, true);
  set(TextStyle::Documentation, theme.commentColor, false, true);
  set(TextStyle::Annotation, theme.stringColor, false, true);
  set(TextStyle::CommentVar, theme.commentColor, false, true);
  set(TextStyle::RegionMarker, theme.commentColor);
  set(TextStyle::Information, theme.functionColor);
  set(TextStyle::Warning, theme.numberColor);
  set(TextStyle::Alert, theme.variableColor);
  set(TextStyle::Error, theme.variableColor);

  return map;
}

QString highlight(const QString &code, const QString &language, const StyleMap &styles, bool isDark,
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

} // namespace vicinae::document::syntax
