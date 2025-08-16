#pragma once
#include "ui/image/image.hpp"
#include "ui/selectable-omni-list-widget/selectable-omni-list-widget.hpp"
#include "ui/typography/typography.hpp"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <qpainter.h>

class TransformResult : public SelectableOmniListWidget {
  QSize m_arrowMid;
  bool m_isDividerVisible;
  QWidget *m_base;
  QWidget *m_result;
  ImageWidget *m_arrowIcon = new ImageWidget(this);

  QWidget *createVContainer(QWidget *left, QWidget *right) const;
  TypographyWidget *createLabel(const QString &text, QWidget *parent = nullptr) const;

protected:
  void paintEvent(QPaintEvent *event) override;
  int availableHeight() const;
  void selectionChanged(bool selected) override;

public:
  QSize sizeHint() const override;
  void setDividerVisible(bool visible);

  void setBase(QWidget *widget, const QString &chip);
  void setBase(const QString &text, const QString &chip);

  void setResult(QWidget *widget, const QString &chip);
  void setResult(const QString &text, const QString &chip);

  TransformResult();
};
