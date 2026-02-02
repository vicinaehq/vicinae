#include "../image/url.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "theme.hpp"
#include "ui/image/image.hpp"
#include "ui/typography/typography.hpp"
#include "utils/layout.hpp"
#include <google/protobuf/descriptor.h>
#include <qnamespace.h>
#include <qurl.h>
#include <qwidget.h>

class TextLinkWidget : public QWidget {
  ImageWidget *m_icon = new ImageWidget;
  TypographyWidget *m_text = new TypographyWidget;
  QUrl m_href;

  void setArrowIcon(SemanticColor color) { m_icon->setUrl(ImageURL::builtin("arrow-ne").setFill(color)); }

  bool event(QEvent *event) override {
    switch (event->type()) {
    case QEvent::HoverEnter:
      setArrowIcon(SemanticColor::Foreground);
      break;
    case QEvent::HoverLeave:
      setArrowIcon(SemanticColor::TextMuted);
      break;
    default:
      break;
    }

    return QWidget::event(event);
  }

  void mousePressEvent(QMouseEvent *event) override {
    if (event->button() == Qt::LeftButton) {
      ServiceRegistry::instance()->appDb()->openTarget(m_href.toString());

      return;
    }

    QWidget::mousePressEvent(event);
  }

  void setupUI() {
    m_icon->setFixedSize(20, 20);
    setAttribute(Qt::WA_Hover);
    setArrowIcon(SemanticColor::TextMuted);
    HStack().add(m_text).add(m_icon).spacing(5).imbue(this);
  }

public:
  void setText(const QString &text) { m_text->setText(text); }
  void setHref(const QUrl &href) { m_href = href; }

  TextLinkWidget(const QString &text, const QUrl &href) {
    setupUI();
    m_text->setText(text);
    m_href = href;
  }

  TextLinkWidget() { setupUI(); }
};
