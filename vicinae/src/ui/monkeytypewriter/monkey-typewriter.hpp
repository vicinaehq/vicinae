#include <QWidget>
#include <chrono>
#include <qevent.h>

class MonkeyTypeWriter : public QWidget {
public:
  MonkeyTypeWriter();

  void setQuote(QStringView quote);
  void start();
  void stop();
  void restart();
  void setBlinkingCursor(bool value);
  void setBlinkingCursorInterval(std::chrono::milliseconds ms);

protected:
  void keyPressEvent(QKeyEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  QSize sizeHint() const override;
  void resizeEvent(QResizeEvent *event) override {
    qDebug() << "resize" << event->size();
    renderText();
    QWidget::resizeEvent(event);
  }

private:
  static constexpr const std::chrono::milliseconds m_defaultBlinkingInterval = std::chrono::milliseconds(500);
  enum class CharState {
    Untyped,
    Mistyped,
    Typed,
  };

  struct CharGroup {
    QString str;
    CharState state;
    size_t cursor;
    size_t endCursor;
  };

  void renderText();

  QPixmap m_renderedText;
  bool m_cursorShown = false;
  QStringView m_quote;
  std::vector<CharGroup> m_charGroups;
  size_t m_cursor = 0;
  QTimer m_timer;
  QTimer m_typingDebounce;

  bool m_isTyping = false;
};
