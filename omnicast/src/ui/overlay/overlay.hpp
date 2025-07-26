#pragma once
#include <qwidget.h>

class ApplicationContext;

class OverlayView : public QWidget {
  ApplicationContext *m_ctx = nullptr;

public:
  void dismiss();
  void setContext(ApplicationContext *ctx);
};
