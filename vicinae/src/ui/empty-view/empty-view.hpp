#pragma once
#include "ui/image/omnimg.hpp"
#include <QWidget>

class TypographyWidget;

class EmptyViewWidget : public QWidget {
  Omnimg::ImageWidget *m_icon = new Omnimg::ImageWidget(this);
  TypographyWidget *m_title;
  TypographyWidget *m_description;

  void setupUi();

public:
  void setTitle(const QString &title);
  void setDescription(const QString &description);
  void setIcon(const std::optional<ImageURL> url);

  EmptyViewWidget(QWidget *parent = nullptr);
};
