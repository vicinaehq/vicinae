#pragma once
#include "ui/form/base-input.hpp"
#include "ui/omni-tree/omni-tree.hpp"
#include "ui/typography/typography.hpp"
#include "ui/shortcut-recorder-input/shortcut-recorder-input.hpp"
#include <qcoreevent.h>
#include <qgraphicseffect.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpainterpath.h>
#include <qwidget.h>

class KeybindSettingsView : public QWidget {
public:
  KeybindSettingsView();

protected:
  void resizeEvent(QResizeEvent *event) override;
  void textChanged(const QString &text);
  bool eventFilter(QObject *sender, QEvent *event) override;
  void selectionUpdated(VirtualTreeItemDelegate *next, VirtualTreeItemDelegate *prev);
  void setupUI();

private:
  QWidget *m_leftPane = new QWidget;
  QWidget *m_rightPane = new QWidget;
  BaseInput *m_input = new BaseInput;
  OmniTree *m_tree = new OmniTree;

  // side pannel
  ImageWidget *m_currentIcon = new ImageWidget;
  TypographyWidget *m_currentName = new TypographyWidget;
  TypographyWidget *m_currentDescription = new TypographyWidget;

  static constexpr size_t HEIGHT = 60;
};
