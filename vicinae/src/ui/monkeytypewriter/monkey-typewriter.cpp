#include "monkey-typewriter.hpp"
#include "theme/colors.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <numbers>
#include <qevent.h>
#include <qnamespace.h>
#include <qobjectdefs.h>
#include <ranges>

static const std::vector<QString> quotes = {
    "never child house see so house out just go develop person can large day up place feel point more off "
    "have only early around state life eye there who because play large head home never public old lead get "
    "problem still with year"};

const QString QUOTE =
    R"(neverrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr do it please or else... You know what's gonna happen. Yeah you know bro.
)";

MonkeyTypeWriter::MonkeyTypeWriter() {
  auto font = QFont();
  font.setPointSize(16);
  font.setHintingPreference(QFont::PreferFullHinting); // prevent flickering
  setFont(font);
  setQuote(QUOTE);
  setFocusPolicy(Qt::StrongFocus);
  setBlinkingCursorInterval(m_defaultBlinkingInterval);

  connect(&m_timer, &QTimer::timeout, this, [this]() {
    m_cursorShown = !m_cursorShown;
    renderText();
  });
}

void MonkeyTypeWriter::setBlinkingCursorInterval(std::chrono::milliseconds ms) { m_timer.setInterval(ms); }

QSize MonkeyTypeWriter::sizeHint() const {
  QFontMetrics fm = fontMetrics();
  QSize hint = fm.boundingRect(0, 0, width(), 0, Qt::TextWordWrap | Qt::AlignLeft, m_quote.toString()).size();

  qDebug() << "size hint" << hint;

  return hint;
}

void MonkeyTypeWriter::keyPressEvent(QKeyEvent *event) {
  // do not block escape
  if (event->key() == Qt::Key_Escape) { return QWidget::keyPressEvent(event); }

  if (event->key() == Qt::Key_Delete) {
    qDebug() << "delete TBD";
    return;
  }

  QString text = event->text();

  QStringView refText = m_quote.sliced(m_cursor, text.size());
  CharState state = refText == text ? CharState::Typed : CharState::Mistyped;

  qDebug() << "ref" << refText << "text" << text;

  if (m_charGroups.empty() || m_charGroups.back().state != state) {
    if (!m_charGroups.empty()) { m_charGroups.back().endCursor = m_cursor; }
    m_charGroups.emplace_back(CharGroup{.str = refText.toString(),
                                        .state = state,
                                        .cursor = m_cursor,
                                        .endCursor = m_cursor + refText.size()});
  } else {
    m_charGroups.back().str += refText.toString();
    m_charGroups.back().endCursor += refText.size();
  }

  m_cursor += refText.size();
  renderText();
}

void MonkeyTypeWriter::renderText() {
  QPixmap canva(size());
  canva.fill(Qt::transparent);
  OmniPainter painter(&canva);

  painter.setFont(font());

  qDebug() << "render on cava" << canva;

  QFontMetrics fm = fontMetrics();

  QColor untypedColor = painter.resolveColor(SemanticColor::TextPrimary);
  QPoint point(0, fm.ascent());
  size_t cursorIdx = 0;

  QPoint cursorPoint;
  size_t groupIdx = 0;

  std::vector<QStringView> groups;

  std::function<void(const QString &text, size_t endCursor)> appendText = [&](const QString &text,
                                                                              size_t endCursor) {
    if (text.isEmpty()) return;

    auto words = text.split(" ");

    for (auto [idx, word] : words | std::views::enumerate) {
      int advance = fm.horizontalAdvance(word);
      int fullAdvance = advance;
      bool newline = false;
      bool isLast = idx == words.size() - 1;
      bool isFirst = idx == 0;
      bool preventCut = false;

      if (advance > canva.width()) {
        int n = canva.width() / fm.averageCharWidth();
        int nparts = word.size() / n;

        for (auto ch : word) {
          int advance = fm.horizontalAdvance(ch);

          if (point.x() + advance > canva.width()) { point = QPoint(0, point.y() + fm.lineSpacing()); }

          painter.drawText(point, ch);
          point = QPoint(point.x() + advance, point.y());
        }
        continue;
      }

      // we need to get the word we are in
      if (isFirst) {
        qDebug() << "first word";
        size_t cursor = endCursor;
        int low = 0;
        int high = 0;

        for (int i = cursor; i >= 0; --i) {
          if (m_quote[i] == ' ') {
            low = i;
            break;
          }
        }

        for (int i = cursor; i < m_quote.size(); ++i) {
          if (m_quote[i] == ' ') {
            high = i;
            break;
          }
        }

        QString fullWord = m_quote.sliced(low, high - low).toString();
        qDebug() << "got isolated full word" << fullWord;
        int fullAdv = fm.horizontalAdvance(fullWord);

        if (fullAdv > canva.width()) { preventCut = true; }

      }

      // last "word" might not be complete, so we need to get the full word
      // to make sure it fits on the current line to write the partial.
      else if (isLast && endCursor) {
        size_t cursor = endCursor;
        size_t quoteIdx = m_quote.indexOf(' ', cursor);
        QString fullWord =
            word + m_quote.sliced(cursor, (quoteIdx != -1 ? quoteIdx : m_quote.size()) - cursor);
        int fullAdv = fm.horizontalAdvance(fullWord);

        if (fullAdv < canva.width()) { fullAdvance = fm.horizontalAdvance(fullWord); }
        qDebug() << "is last, full word is" << fullWord;
      }

      if (!preventCut && point.x() + fullAdvance > canva.width()) {
        point = QPoint(0, point.y() + fm.lineSpacing());
        newline = true;
      }

      if (newline || idx == 0) {
        painter.drawText(point, word);
        point = QPoint(point.x() + advance, point.y());
      } else {
        QString txt = " " + word;
        painter.drawText(point, txt);
        point = QPoint(point.x() + fm.horizontalAdvance(txt), point.y());
      }
    }
  };

  painter.setPen(QPen(untypedColor));
  untypedColor.setAlphaF(0.5);

  for (const auto &grp : m_charGroups) {
    if (grp.state == CharState::Typed) {
      painter.setThemePen(SemanticColor::TextPrimary);
    } else if (grp.state == CharState::Mistyped) {
      painter.setThemePen(SemanticColor::Red);
    }

    appendText(grp.str, grp.endCursor);
    ++groupIdx;
  }

  painter.setThemePen(untypedColor);
  appendText(m_quote.sliced(m_cursor).toString(), 0);

  if (m_cursorShown) {
    painter.setThemeBrush(SemanticColor::Accent);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(QRect(cursorPoint.x(), cursorPoint.y() - fm.ascent(), 2, fm.ascent()), 6, 6);
  }

  m_renderedText = canva;
  update();
}

void MonkeyTypeWriter::paintEvent(QPaintEvent *event) {
  if (m_renderedText.size() != size()) return;

  QPainter painter(this);
  painter.drawPixmap(0, 0, m_renderedText);
}

void MonkeyTypeWriter::setQuote(QStringView quote) {
  m_quote = quote;
  m_cursor = 0;
  renderText();
}
