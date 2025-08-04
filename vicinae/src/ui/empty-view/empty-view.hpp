#pragma once
#include "ui/image/image.hpp"
#include <QWidget>

class TypographyWidget;

class EmptyViewWidget : public QWidget {
  ImageWidget *m_icon = new ImageWidget(this);
  TypographyWidget *m_title;
  TypographyWidget *m_description;

  void setupUi();

public:
  void setTitle(const QString &title);
  void setDescription(const QString &description);
  void setIcon(const std::optional<ImageURL> url);

  EmptyViewWidget(QWidget *parent = nullptr);
};
