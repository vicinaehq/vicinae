#pragma once
#include "extend/empty-view-model.hpp"
#include "image-viewer.hpp"
#include "ui/text-label.hpp"
#include <qboxlayout.h>
#include <qwidget.h>

class EmptyViewWidget : public QWidget {
public:
  EmptyViewWidget(const EmptyViewModel &model) {
    auto container = new QVBoxLayout();

    container->setAlignment(Qt::AlignCenter);

    auto layout = new QVBoxLayout();

    if (model.icon) {
      layout->addWidget(ImageViewer::createFromModel(*model.icon, {64, 64}), 0,
                        Qt::AlignCenter);
    }

    layout->addWidget(new QLabel(model.title), 0, Qt::AlignCenter);
    layout->addWidget(new TextLabel(model.description), 0, Qt::AlignCenter);

    container->addLayout(layout);
    setLayout(container);
  }
};
