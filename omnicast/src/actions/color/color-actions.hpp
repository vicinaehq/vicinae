#pragma once
#include "color-formatter.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"

class CopyColorAs : public AbstractAction {
  ColorFormatter::ColorFormat m_format;
  QColor m_color;

  void execute() override {
    ColorFormatter formatter;
    auto ui = ServiceRegistry::instance()->UI();
    auto clipman = ServiceRegistry::instance()->clipman();
    auto formatted = formatter.format(m_color, m_format);

    clipman->copyText(formatted);
    ui->closeWindow();
  }

  QString id() const override { return QString::number(m_format); };

  QString title() const override {
    ColorFormatter m_formatter;
    QString formatName = m_formatter.formatName(m_format);

    return QString("Copy as %1").arg(formatName);
  }

public:
  CopyColorAs(const QColor &color, ColorFormatter::ColorFormat format)
      : AbstractAction("Copy color as", BuiltinOmniIconUrl("copy-clipboard")), m_color(color),
        m_format(format) {}
};
