#pragma once
#include "extend/tag-model.hpp"
#include "image-viewer.hpp"
#include "theme.hpp"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qwidget.h>

class Tag : public QWidget {
  QHBoxLayout *layout;
  QLabel *label;

  int calculateLuminance(QColor color) {
    return 0.2126 * color.redF() + 0.7152 * color.greenF() + 0.0722 * color.blueF();
  }

  QColor computeBackground(QColor main) {
    /*
auto luminance = calculateLuminance(main);
double factor = luminance > 0.5 ? 0.7 : 1.5;

auto hsl = main.toHsl();

hsl.setHsl(hsl.hue(), hsl.saturation(), hsl.lightness() * 1.5);
  */

    QColor bg(main);

    bg.setAlphaF(0.1);

    return bg;
  }

public:
  Tag() : layout(new QHBoxLayout), label(new QLabel) {
    setProperty("class", "tag");
    layout->setContentsMargins(5, 5, 5, 5);
    layout->addWidget(label, 0, Qt::AlignCenter);
    setLayout(layout);
  }

  void applyModel(const TagItemModel &model) {
    ThemeService theme;

    setText(model.text);

    if (model.color) {
      // setColor(theme.getColor(*model.color));
    }
    if (model.icon) {
      auto img = ImageViewer::createFromModel(*model.icon, {16, 16});

      addLeftWidget(img);
    }
  }

  void setText(const QString &text) { label->setText(text); }

  void setColor(QColor color) {
    auto bg = computeBackground(color);

    QString s = R"(
		.tag {
			border-radius: 5px;
			background-color: %2;
		}

		.tag QLabel {
			color: %1;
		}
	)";

    setStyleSheet(s.arg(color.name()).arg(bg.name(QColor::HexArgb)));
  }

  void addLeftWidget(QWidget *left) { layout->insertWidget(0, left, 0, Qt::AlignCenter); }
};

class TagList : public QWidget {
  QHBoxLayout *layout;

public:
  TagList() : layout(new QHBoxLayout) {
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
  }

  void addTag(Tag *tag) { layout->addWidget(tag); }
};
