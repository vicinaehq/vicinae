#include "common.hpp"
#include "ui/focus-notifier.hpp"
#include <qcoreevent.h>
#include <qevent.h>
#include <qobject.h>
#include <qwidget.h>

class QPlainTextEdit;

class TextArea : public JsonFormItemWidget {
  int m_rows = 3;

public:
  TextArea(QWidget *m_parent = nullptr);

  QJsonValue asJsonValue() const override;
  void setValueAsJson(const QJsonValue &value) override;
  void setText(const QString &text);
  QString text() const;
  FocusNotifier *focusNotifier() const override { return m_notifier; }
  void setPlaceholderText(const QString &text);
  void setTabSetFocus(bool value);

  void setGrowAsRequired(bool value);
  void setRows(size_t rowCount);
  void setMargins(int margins);

  /**
   * If this is true (the default) the text area will expand as the text
   * document becomes higher and shrink as it becomes smaller.
   * The minimum height can be set by calling `setRows` to specify a minimum number of rows.
   * If false, a scroll bar will be shown if the text becomes to tall to fit in
   * the allowed space.
   * For UX reasons, the former is always preferred as nested scrollbars can make it diffcult
   * to understand what is going on.
   */
  bool growAsRequired() const;

  /**
   * Whether tab should be ignored (the default) or be inserted as part of the text.
   */
  bool ignoreTab() const;

protected:
  void resizeEvent(QResizeEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  int heightForRowCount(int rowCount);

private:
  void resizeArea();

  QPlainTextEdit *m_textEdit = nullptr;
  FocusNotifier *m_notifier = new FocusNotifier(this);
  bool m_growAsRequired = false;

  void setupUI();
};
