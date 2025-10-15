#pragma once
#include "ui/views/list-view.hpp"

class ManageThemesView : public ListView {
public:
  ManageThemesView();

private:
  void generateList(const QString &query);
  void initialize() override;
  void textChanged(const QString &s) override;
};
