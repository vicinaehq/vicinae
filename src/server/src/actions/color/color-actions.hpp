#pragma once
#include "color-formatter.hpp"
#include "common.hpp"
#include "../../ui/image/url.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"

class CopyColorAs : public AbstractAction {
  ColorFormatter::ColorFormat m_format;
  QColor m_color;

  void execute(ApplicationContext *ctx) override {
    ColorFormatter formatter;
    auto clipman = ctx->services->clipman();
    auto formatted = formatter.format(m_color, m_format);

    clipman->copyText(formatted);
  }

  QString id() const override { return QString::number(m_format); };

  QString title() const override {
    ColorFormatter m_formatter;
    QString formatName = m_formatter.formatName(m_format);

    return QString("Copy as %1").arg(formatName);
  }

public:
  CopyColorAs(const QColor &color, ColorFormatter::ColorFormat format)
      : AbstractAction("Copy color as", ImageURL::builtin("copy-clipboard")), m_color(color),
        m_format(format) {}
};
