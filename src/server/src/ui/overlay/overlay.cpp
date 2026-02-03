#include "common/context.hpp"
#include "ui/overlay/overlay.hpp"
#include "overlay-controller/overlay-controller.hpp"

void OverlayView::setContext(ApplicationContext *context) { m_ctx = context; }

void OverlayView::dismiss() { m_ctx->overlay->dismissCurrent(); }
