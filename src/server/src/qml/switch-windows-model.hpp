#pragma once
#include "fuzzy-list-model.hpp"
#include "services/app-service/abstract-app-db.hpp"
#include "services/window-manager/abstract-window-manager.hpp"

struct WindowEntry {
  AbstractWindowManager::WindowPtr window;
  std::shared_ptr<AbstractApplication> app; // may be null
};

template <> struct fuzzy::FuzzySearchable<WindowEntry> {
  static int score(const WindowEntry &e, std::string_view query) {
    auto title = e.window->title().toStdString();
    auto wmClass = e.window->wmClass().toStdString();
    if (e.app) {
      auto appName = e.app->displayName().toStdString();
      return fuzzy::scoreWeighted({{title, 1.0}, {wmClass, 0.7}, {appName, 0.9}}, query);
    }
    return fuzzy::scoreWeighted({{title, 1.0}, {wmClass, 0.7}}, query);
  }
};

class SwitchWindowsModel : public FuzzyListModel<WindowEntry> {
  Q_OBJECT

public:
  using FuzzyListModel::FuzzyListModel;

protected:
  QString displayTitle(const WindowEntry &e) const override;
  QString displaySubtitle(const WindowEntry &e) const override;
  QString displayIconSource(const WindowEntry &e) const override;
  QString displayAccessory(const WindowEntry &e) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const WindowEntry &e) const override;
  QString sectionLabel() const override;
};
