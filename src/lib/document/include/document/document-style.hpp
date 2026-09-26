#pragma once

#include <QColor>
#include <QObject>
#include <QProperty>
#include <QString>
#include <QtQml/qqmlregistration.h>

namespace vicinae::document {

class DocumentStyle : public QObject {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(QString fontFamily READ default WRITE default BINDABLE bindableFontFamily)
  Q_PROPERTY(QString monoFontFamily READ default WRITE default BINDABLE bindableMonoFontFamily)
  Q_PROPERTY(qreal regularFontSize READ default WRITE default BINDABLE bindableRegularFontSize)
  Q_PROPERTY(qreal smallerFontSize READ default WRITE default BINDABLE bindableSmallerFontSize)
  Q_PROPERTY(bool isDark READ default WRITE default BINDABLE bindableIsDark)
  Q_PROPERTY(QColor foreground READ default WRITE default BINDABLE bindableForeground)
  Q_PROPERTY(QColor textMuted READ default WRITE default BINDABLE bindableTextMuted)
  Q_PROPERTY(QColor divider READ default WRITE default BINDABLE bindableDivider)
  Q_PROPERTY(QColor secondaryBackground READ default WRITE default BINDABLE bindableSecondaryBackground)
  Q_PROPERTY(QColor textSelectionBg READ default WRITE default BINDABLE bindableTextSelectionBg)
  Q_PROPERTY(QColor textSelectionFg READ default WRITE default BINDABLE bindableTextSelectionFg)
  Q_PROPERTY(QColor accent READ default WRITE default BINDABLE bindableAccent)
  Q_PROPERTY(QColor linkColor READ default WRITE default BINDABLE bindableLinkColor)
  Q_PROPERTY(QColor codeBackground READ default WRITE default BINDABLE bindableCodeBackground)
  Q_PROPERTY(QColor codeBorder READ default WRITE default BINDABLE bindableCodeBorder)
  Q_PROPERTY(QColor inlineCodeBackground READ default WRITE default BINDABLE bindableInlineCodeBackground)
  Q_PROPERTY(QColor tableBorder READ default WRITE default BINDABLE bindableTableBorder)
  Q_PROPERTY(QColor tableHeaderBackground READ default WRITE default BINDABLE bindableTableHeaderBackground)
  Q_PROPERTY(QColor scrollBarColor READ default WRITE default BINDABLE bindableScrollBarColor)
  Q_PROPERTY(QColor infoColor READ default WRITE default BINDABLE bindableInfoColor)
  Q_PROPERTY(QColor warningColor READ default WRITE default BINDABLE bindableWarningColor)
  Q_PROPERTY(QColor dangerColor READ default WRITE default BINDABLE bindableDangerColor)
  Q_PROPERTY(QColor successColor READ default WRITE default BINDABLE bindableSuccessColor)
  Q_PROPERTY(QColor keywordColor READ default WRITE default BINDABLE bindableKeywordColor)
  Q_PROPERTY(QColor functionColor READ default WRITE default BINDABLE bindableFunctionColor)
  Q_PROPERTY(QColor variableColor READ default WRITE default BINDABLE bindableVariableColor)
  Q_PROPERTY(QColor builtinColor READ default WRITE default BINDABLE bindableBuiltinColor)
  Q_PROPERTY(QColor numberColor READ default WRITE default BINDABLE bindableNumberColor)
  Q_PROPERTY(QColor stringColor READ default WRITE default BINDABLE bindableStringColor)
  Q_PROPERTY(QColor commentColor READ default WRITE default BINDABLE bindableCommentColor)

public:
  explicit DocumentStyle(QObject *parent = nullptr);
  static DocumentStyle *defaults();
  QBindable<QString> bindableFontFamily() { return &fontFamily; }
  QBindable<QString> bindableMonoFontFamily() { return &monoFontFamily; }
  QBindable<qreal> bindableRegularFontSize() { return &regularFontSize; }
  QBindable<qreal> bindableSmallerFontSize() { return &smallerFontSize; }
  QBindable<bool> bindableIsDark() { return &isDark; }
  QBindable<QColor> bindableForeground() { return &foreground; }
  QBindable<QColor> bindableTextMuted() { return &textMuted; }
  QBindable<QColor> bindableDivider() { return &divider; }
  QBindable<QColor> bindableSecondaryBackground() { return &secondaryBackground; }
  QBindable<QColor> bindableTextSelectionBg() { return &textSelectionBg; }
  QBindable<QColor> bindableTextSelectionFg() { return &textSelectionFg; }
  QBindable<QColor> bindableAccent() { return &accent; }
  QBindable<QColor> bindableLinkColor() { return &linkColor; }
  QBindable<QColor> bindableCodeBackground() { return &codeBackground; }
  QBindable<QColor> bindableCodeBorder() { return &codeBorder; }
  QBindable<QColor> bindableInlineCodeBackground() { return &inlineCodeBackground; }
  QBindable<QColor> bindableTableBorder() { return &tableBorder; }
  QBindable<QColor> bindableTableHeaderBackground() { return &tableHeaderBackground; }
  QBindable<QColor> bindableScrollBarColor() { return &scrollBarColor; }
  QBindable<QColor> bindableInfoColor() { return &infoColor; }
  QBindable<QColor> bindableWarningColor() { return &warningColor; }
  QBindable<QColor> bindableDangerColor() { return &dangerColor; }
  QBindable<QColor> bindableSuccessColor() { return &successColor; }
  QBindable<QColor> bindableKeywordColor() { return &keywordColor; }
  QBindable<QColor> bindableFunctionColor() { return &functionColor; }
  QBindable<QColor> bindableVariableColor() { return &variableColor; }
  QBindable<QColor> bindableBuiltinColor() { return &builtinColor; }
  QBindable<QColor> bindableNumberColor() { return &numberColor; }
  QBindable<QColor> bindableStringColor() { return &stringColor; }
  QBindable<QColor> bindableCommentColor() { return &commentColor; }

  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QString, fontFamily)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QString, monoFontFamily)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, qreal, regularFontSize)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, qreal, smallerFontSize)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, bool, isDark)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, foreground)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, textMuted)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, divider)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, secondaryBackground)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, textSelectionBg)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, textSelectionFg)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, accent)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, linkColor)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, codeBackground)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, codeBorder)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, inlineCodeBackground)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, tableBorder)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, tableHeaderBackground)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, scrollBarColor)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, infoColor)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, warningColor)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, dangerColor)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, successColor)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, keywordColor)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, functionColor)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, variableColor)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, builtinColor)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, numberColor)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, stringColor)
  Q_OBJECT_BINDABLE_PROPERTY(DocumentStyle, QColor, commentColor)
};

} // namespace vicinae::document
