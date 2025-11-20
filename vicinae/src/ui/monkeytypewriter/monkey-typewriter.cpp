#include "monkey-typewriter.hpp"
#include "theme/colors.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <qevent.h>
#include <qnamespace.h>
#include <qobjectdefs.h>
#include <ranges>

static const std::vector<QString> quotes = {
    "never child house see so house out just go develop person can large day up place feel point more off "
    "have only early around state life eye there who because play large head home never public old lead get "
    "problem still with year"};

MonkeyTypeWriter::MonkeyTypeWriter() {
  auto font = QFont();
  font.setPointSize(16);
  font.setHintingPreference(QFont::PreferFullHinting); // prevent flickering
  setFont(font);
  setQuote(quotes.front());
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
    m_charGroups.emplace_back(CharGroup{.str = refText.toString(), .state = state, .cursor = m_cursor});
  } else {
    m_charGroups.back().str += refText.toString();
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

  bool isPendingWord = false;

  size_t groupIdx = 0;

  /*
  std::function<void(const QString &text)> appendText = [&](const QString &text) {
    if (text.isEmpty()) return;
    int advance = fm.horizontalAdvance(text);
    int lineAdvance = advance;

    if (m_charGroups.size() > groupIdx + 1) {
      QString nextFullWord = text.split(' ').last() + m_charGroups[groupIdx + 1].str.split(' ').first();
      lineAdvance = fm.horizontalAdvance(nextFullWord);
    }

    bool isNextWord = m_charGroups.size() > groupIdx + 1 && m_charGroups[groupIdx + 1].str.startsWith(' ');

    if (point.x() + lineAdvance > canva.width()) {
      for (int i = text.size() - 1; i >= 0; --i) {
        auto ch = text.at(i);
        if (ch == ' ') {
          QString s = text.sliced(0, i);
          int advance = fm.horizontalAdvance(s);
          if (point.x() + advance < canva.width()) {
            appendText(s);
            ++cursorIdx;
            point = QPoint(0, point.y() + fm.lineSpacing());
            appendText(text.sliced(i + 1));
            return;
          }
        }
      }

      // qDebug() << "line jump" << "point" << point << "width" << width();
      point = QPoint(0, point.y() + fm.lineSpacing());
      ++cursorIdx;
    }

    for (const auto &[idx, ch] : text | std::views::enumerate) {
      if (cursorIdx == m_cursor) {
        cursorPoint =
            QPoint(point.x() + (idx > 0 ? fm.horizontalAdvance(text.sliced(0, idx + 1)) : 0), point.y());
      }
      ++cursorIdx;
    }

    painter.drawText(point, text);
    point = QPoint(point.x() + advance, point.y());
  };
  */

  std::function<void(const QString &text)> appendText = [&](const QString &text) {
    if (text.isEmpty()) return;
    int advance = fm.horizontalAdvance(text);
    int lineAdvance = advance;

    auto words = text.split(" ");

    if (point.x() + lineAdvance > canva.width()) {}

    point = QPoint(0, point.y() + fm.lineSpacing());

    for (const auto &[idx, ch] : text | std::views::enumerate) {
      if (cursorIdx == m_cursor) {
        cursorPoint =
            QPoint(point.x() + (idx > 0 ? fm.horizontalAdvance(text.sliced(0, idx + 1)) : 0), point.y());
      }
      ++cursorIdx;
    }

    painter.drawText(point, text);
    point = QPoint(point.x() + advance, point.y());
  };

  painter.setPen(QPen(untypedColor));
  untypedColor.setAlphaF(0.5);

  for (const auto &grp : m_charGroups) {
    if (grp.state == CharState::Typed) {
      painter.setThemePen(SemanticColor::TextPrimary);
    } else if (grp.state == CharState::Mistyped) {
      painter.setThemePen(SemanticColor::Red);
    }

    appendText(grp.str);

    ++groupIdx;
    // appendText(grp.str);
  }

  painter.setThemePen(untypedColor);
  appendText(m_quote.sliced(m_cursor).toString());

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
