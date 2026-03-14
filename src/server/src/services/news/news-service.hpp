#pragma once
#include "ui/action-pannel/action.hpp"
#include "ui/image/url.hpp"
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <QObject>

class ActionPanelState;

namespace config {
class Manager;
}

struct NewsItem {
  std::string id;
  QString title;
  QString subtitle;
  ImageURL icon;
  using ActionFactory = std::function<std::unique_ptr<ActionPanelState>(ApplicationContext *)>;
  ActionFactory actionFactory;
};

class DismissNewsAction : public AbstractAction {
public:
  DismissNewsAction(std::string newsId);

  void execute(ApplicationContext *ctx) override;

private:
  std::string m_newsId;
};

class NewsService : public QObject {
  Q_OBJECT

signals:
  void itemsChanged();

public:
  explicit NewsService(config::Manager &config);

  void dismiss(const std::string &id);
  bool isDismissed(const std::string &id) const;
  std::vector<const NewsItem *> activeItems() const;

private:
  void loadState();
  void saveState() const;
  static std::vector<NewsItem> allItems();

  config::Manager &m_config;
  std::filesystem::path m_stateFile;
  std::vector<NewsItem> m_items;
  std::vector<std::string> m_dismissed;
};
