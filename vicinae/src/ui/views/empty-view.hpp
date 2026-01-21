#pragma once
#include "layout.hpp"
#include "ui/empty-view/empty-view.hpp"
#include "ui/image/url.hpp"
#include "ui/views/base-view.hpp"
#include <qnamespace.h>
#include <qobject.h>
#include <qtimer.h>
#include <qwidget.h>

class FocusAnchor : public QWidget {
public:
  FocusAnchor(QWidget *parent) : QWidget(parent) { setFocusPolicy(Qt::StrongFocus); }
};

class EmptyView : public BaseView {
  struct EmptyData {
    std::optional<QString> title;
    std::optional<QString> description;
    std::optional<ImageURL> icon;
  };

public:
  EmptyView() { VStack().add(m_empty).imbue(this); }

  void initialize() override {
    QTimer::singleShot(0, [this]() {
      m_anchor->show();
      m_anchor->setFocus();
    });
  }

  void setData(const EmptyData &model) {
    m_empty->setTitle(model.title.value_or(""));
    m_empty->setDescription(model.description.value_or(""));
    m_empty->setIcon(model.icon);
  }

protected:
  bool supportsSearch() const override { return false; }

private:
  EmptyViewWidget *m_empty = new EmptyViewWidget;
  FocusAnchor *m_anchor = new FocusAnchor(this);
};
