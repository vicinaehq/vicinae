#pragma once
#include "actions/app/app-actions.hpp"
#include "actions/calculator/calculator-actions.hpp"
#include "common.hpp"
#include "layout.hpp"
#include "misc/file-list-item.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/app-service/abstract-app-db.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "services/config/config-service.hpp"
#include "services/files-service/file-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/calculator-service/calculator-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/app-service/app-service.hpp"
#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include "ui/search-bar/search-bar.hpp"
#include "ui/transform-result/transform-result.hpp"
#include "ui/views/base-view.hpp"
#include "ui/vlist/vlist.hpp"
#include "ui/vlist/common/section-model.hpp"
#include "utils.hpp"
#include <absl/strings/str_format.h>
#include <qfuturewatcher.h>
#include <qlocale.h>
#include <qobjectdefs.h>
#include <variant>

// clang-format off
enum class SectionType { 
	Link, Calculator, Results, Files, Fallback,
	Favorites
};
// clang-format on

struct FallbackItem {
  const RootItem *item;
};

struct FavoriteItem {
  const RootItem *item;
};

struct LinkItem {
  std::shared_ptr<AbstractApplication> app;
  QString url;
};

/**
 * A model that produces actionnable items, that is items that generate an action panel
 * and an optional completer.
 */
template <typename T> class ListModelActionnable {
  virtual std::unique_ptr<ActionPanelState> createActionPanel(const T &item,
                                                              ApplicationContext *ctx) const = 0;
  virtual std::unique_ptr<CompleterData> createCompleter(const T &item) const = 0;
};

using RootItemVariant = std::variant<AbstractCalculatorBackend::CalculatorResult, const RootItem *,
                                     std::filesystem::path, FallbackItem, FavoriteItem, LinkItem>;

class RootSearchModel : public vicinae::ui::SectionListModel<RootItemVariant, SectionType>,
                        public ListModelActionnable<RootItemVariant> {
  using FileSearchWatcher = QFutureWatcher<std::vector<IndexerFileResult>>;

public:
  RootSearchModel(RootItemManager *manager, FileService *fs, AppService *appDb, CalculatorService *calculator)
      : m_manager(manager), m_fs(fs), m_appDb(appDb), m_calculator(calculator) {
    using namespace std::chrono_literals;

    regenerateFallback();
    regenerateFavorites();
    m_fileSearchDebounce.setInterval(100ms);
    m_fileSearchDebounce.setSingleShot(true);
    m_calculatorDebounce.setInterval(100ms);
    m_calculatorDebounce.setSingleShot(true);

    connect(&m_fileSearchDebounce, &QTimer::timeout, this, &RootSearchModel::startFileSearch);
    connect(&m_calculatorDebounce, &QTimer::timeout, this, &RootSearchModel::startCalculator);
    connect(&m_calcWatcher, &QFutureWatcher<AbstractCalculatorBackend::ComputeResult>::finished, this,
            &RootSearchModel::handleCalculatorFinished);
    connect(m_manager, &RootItemManager::fallbackDisabled, this, &RootSearchModel::regenerateFallback);
    connect(m_manager, &RootItemManager::fallbackEnabled, this, &RootSearchModel::regenerateFallback);
    connect(m_manager, &RootItemManager::itemsChanged, this, &RootSearchModel::reloadSearch);
    connect(m_manager, &RootItemManager::itemFavoriteChanged, this, [this]() { regenerateFavorites(); });
    connect(&m_fileWatcher, &FileSearchWatcher::finished, this, &RootSearchModel::handleFileSearchFinished);
  }

  std::unique_ptr<ActionPanelState> createActionPanel(const RootItemVariant &item,
                                                      ApplicationContext *ctx) const override {
    const auto visitor = overloads{
        [&](const RootItem *item) {
          return item->newActionPanel(ctx, m_manager->itemMetadata(item->uniqueId()));
        },
        [](const AbstractCalculatorBackend::CalculatorResult &item) {
          auto panel = std::make_unique<ActionPanelState>();
          auto copyAnswer = new CopyCalculatorAnswerAction(item);
          auto copyQA = new CopyCalculatorQuestionAndAnswerAction(item);
          auto putAnswerInSearchBar = new PutCalculatorAnswerInSearchBar(item);
          auto openHistory = new OpenCalculatorHistoryAction();
          auto main = panel->createSection();

          copyAnswer->setPrimary(true);
          main->addAction(copyAnswer);
          main->addAction(copyQA);
          main->addAction(putAnswerInSearchBar);
          main->addAction(openHistory);

          return panel;
        },
        [&](const FallbackItem &item) {
          return item.item->fallbackActionPanel(ctx, m_manager->itemMetadata(item.item->uniqueId()));
        },
        [&](const std::filesystem::path &path) { return FileListItemBase::actionPanel(path, m_appDb); },
        [&](const FavoriteItem &item) {
          return item.item->newActionPanel(ctx, m_manager->itemMetadata(item.item->uniqueId()));
        },
        [&](const LinkItem &item) {
          auto panel = std::make_unique<ActionPanelState>();
          auto section = panel->createSection();
          auto open =
              new OpenAppAction(item.app, QString("Open in %1").arg(item.app->displayName()), {item.url});
          open->setClearSearch(true);
          section->addAction(open);
          return panel;
        }};

    return std::visit(visitor, item);
  }

  std::unique_ptr<CompleterData> createCompleter(const RootItemVariant &item) const override {
    auto rootItemCompleter = [](const RootItem *item) -> std::unique_ptr<CompleterData> {
      ArgumentList args = item->arguments();

      if (args.empty()) return nullptr;

      auto cmpl = std::make_unique<CompleterData>();
      cmpl->arguments = args;
      cmpl->iconUrl = item->iconUrl();

      return cmpl;
    };

    const auto visitor = overloads{[&](const RootItem *item) { return rootItemCompleter(item); },
                                   [&](const FavoriteItem &item) { return rootItemCompleter(item.item); },
                                   [](auto &&a) { return std::unique_ptr<CompleterData>(); }};

    return std::visit(visitor, item);
  }

  bool shouldFastTrack(const RootItemVariant &item) const {
    const auto handleRootItem = [&](const RootItem *item) {
      if (!item->supportsAliasSpaceShortcut()) return false;

      auto alias = m_manager->itemMetadata(item->uniqueId()).alias;
      return !alias.empty() && alias.starts_with(query);
    };

    const auto visitor = overloads{[&](const RootItem *item) { return handleRootItem(item); },
                                   [&](const FavoriteItem &item) { return handleRootItem(item.item); },
                                   [&](auto &&a) { return false; }};

    return std::visit(visitor, item);
  }

  void reloadSearch() { setFilter(query); }

  void regenerateFallback() { m_fallbackItems = m_manager->fallbackItems(); }
  void regenerateFavorites() { m_favorites = m_manager->queryFavorites(); }

  std::span<const SectionType> sections() const {
    if (query.empty()) return m_rootSections;
    return m_searchSections;
  }

  /**
   * Whether root search should also search for files for queries longer
   * than three characters.
   */
  void setFileSearch(bool value) { m_isFileSearchEnabled = value; }

  void setFilter(std::string_view text) {
    query = text;
    m_defaultOpener.reset();
    m_calc.reset();
    m_items = {};
    m_files.clear();

    if (text.empty()) {
      m_items = m_manager->search("", {.includeFavorites = false, .prioritizeAliased = false});
      emit dataChanged();
      return;
    }

    if (text.starts_with('/')) {
      std::error_code ec;
      if (std::filesystem::exists(text, ec)) {
        m_files = {{.path = text}};
        emit dataChanged();
        return;
      }
    }

    if (auto url = QUrl(QString::fromUtf8(text.data(), text.size()));
        url.isValid() && !url.scheme().isEmpty()) {
      if (auto app = m_appDb->findDefaultOpener(query.c_str())) {
        m_defaultOpener = LinkItem{.app = app, .url = query.c_str()};
        emit dataChanged();
        return;
      }
    }

    m_items = m_manager->search(query.c_str());

    m_resultSectionTitle = std::format("Results ({})", m_items.size());
    m_fallbackSectionTitle = std::format("Use \"{}\" with...", query);
    m_calculatorDebounce.start();
    m_fileSearchDebounce.start();

    emit dataChanged();
  }

  int sectionCount() const override { return sections().size(); }

  int sectionItemHeight(SectionType id) const override {
    switch (id) {
    case SectionType::Calculator:
      return CALCULATOR_HEIGHT;
    default:
      return ITEM_HEIGHT;
    }
  }

  SectionType sectionIdFromIndex(int idx) const override { return sections()[idx]; }

  int sectionItemCount(SectionType id) const override {
    switch (id) {
    case SectionType::Link:
      return m_defaultOpener.has_value();
    case SectionType::Calculator:
      return m_calc.has_value();
    case SectionType::Results:
      return m_items.size();
    case SectionType::Files:
      return m_files.size();
    case SectionType::Fallback:
      return m_fallbackItems.size();
    case SectionType::Favorites:
      return m_favorites.size();
    default:
      return 0;
    }
  }

  std::string_view sectionName(SectionType id) const override {
    switch (id) {
    case SectionType::Link:
      return "Link";
    case SectionType::Calculator:
      return "Calculator";
    case SectionType::Results:
      return query.empty() ? std::string_view("Suggestions") : m_resultSectionTitle;
    case SectionType::Files:
      return "Files";
    case SectionType::Fallback:
      return m_fallbackSectionTitle;

    case SectionType::Favorites:
      return "Favorites";
    }
  }

  RootItemVariant sectionItemAt(SectionType id, int itemIdx) const override {
    switch (id) {
    case SectionType::Link:
      return m_defaultOpener.value();
    case SectionType::Calculator:
      return m_calc.value();
    case SectionType::Results:
      return m_items[itemIdx].item.get().get();
    case SectionType::Files:
      return m_files[itemIdx].path;
    case SectionType::Fallback:
      return FallbackItem{.item = m_fallbackItems[itemIdx].get()};
    case SectionType::Favorites:
      return FavoriteItem{.item = m_favorites[itemIdx].item.get()};
    }

    return {};
  }

  StableID stableId(const RootItemVariant &item) const override {
    static std::hash<QString> hasher = {};
    const auto visitor =
        overloads{[&](const AbstractCalculatorBackend::CalculatorResult &) { return randomId(); },
                  [](const RootItem *item) { return hasher(item->uniqueId()); },
                  [](const LinkItem &item) { return hasher(item.url + ".url"); },
                  [](const std::filesystem::path &path) { return hasher(QString(path.c_str()) + ".files"); },
                  [](const FallbackItem &item) { return hasher(item.item->uniqueId() + ".fallback"); },
                  [](const FavoriteItem &item) { return hasher(item.item->uniqueId() + ".favorite"); }};
    return std::visit(visitor, item);
  }

  WidgetTag widgetTag(const RootItemVariant &item) const override {
    if (std::holds_alternative<AbstractCalculatorBackend::CalculatorResult>(item)) { return InvalidTag; }
    return item.index();
  }

protected:
  WidgetType *createItemWidget(const RootItemVariant &type) const override {
    const auto visitor =
        overloads{[](const AbstractCalculatorBackend::CalculatorResult &) -> WidgetType * {
                    return new TransformResult;
                  },
                  [](const RootItem *) -> WidgetType * { return new DefaultListItemWidget; },
                  [](const std::filesystem::path &path) -> WidgetType * { return new DefaultListItemWidget; },
                  [](const LinkItem &item) -> WidgetType * { return new DefaultListItemWidget; },
                  [](const FallbackItem &item) -> WidgetType * { return new DefaultListItemWidget; },
                  [](const FavoriteItem &item) -> WidgetType * { return new DefaultListItemWidget; }};

    return std::visit(visitor, type);
  }

  void refreshItemWidget(const RootItemVariant &type, WidgetType *widget) const override {
    auto refreshRootItem = [&](const RootItem *item, bool showAlias = true) {
      auto w = static_cast<DefaultListItemWidget *>(widget);
      w->setAlias(showAlias ? m_manager->itemMetadata(item->uniqueId()).alias.c_str() : "");
      w->setName(item->displayName());
      w->setIconUrl(item->iconUrl());
      w->setSubtitle(item->subtitle());
      w->setAccessories(item->accessories());
      w->setActive(item->isActive());
    };
    const auto visitor =
        overloads{[&](const AbstractCalculatorBackend::CalculatorResult &calc) {
                    auto w = static_cast<TransformResult *>(widget);
                    const auto toDp = [](auto &&u) { return u.displayName; };
                    w->setBase(calc.question.text, calc.question.unit.transform(toDp).value_or("Expression"));
                    w->setResult(calc.answer.text, calc.answer.unit.transform(toDp).value_or("Answer"));
                  },
                  [&](const std::filesystem::path &path) {
                    auto w = static_cast<DefaultListItemWidget *>(widget);
                    w->setName(getLastPathComponent(path).c_str());
                    w->setIconUrl(ImageURL::fileIcon(path));
                    w->setSubtitle(path);
                    w->setActive(false);
                  },
                  [&](const LinkItem &item) {
                    auto w = static_cast<DefaultListItemWidget *>(widget);
                    w->setName(item.url);
                    w->setIconUrl(item.app->iconUrl());
                    w->setActive(false);
                  },
                  refreshRootItem,
                  [&](const FallbackItem &item) { refreshRootItem(item.item, false); },
                  [&](const FavoriteItem &item) { refreshRootItem(item.item); }};

    std::visit(visitor, type);
  }

private:
  static constexpr const size_t ITEM_HEIGHT = 41;
  static constexpr const size_t CALCULATOR_HEIGHT = 90;
  static constexpr const int MIN_FS_TEXT_LENGTH = 3;

  static constexpr const std::array<SectionType, 2> m_rootSections = {SectionType::Favorites,
                                                                      SectionType::Results};
  static constexpr const std::array<SectionType, 5> m_searchSections = {
      SectionType::Link, SectionType::Calculator, SectionType::Results, SectionType::Files,
      SectionType::Fallback};

  // fired on calculator debounce timeout
  void startCalculator() {
    if (m_calcWatcher.isRunning()) { m_calcWatcher.cancel(); }

    auto expression = QString::fromStdString(query);

    m_calculatorSearchQuery = query;

    if (expression.startsWith("=") && expression.size() > 1) {
      auto stripped = expression.mid(1);
      m_calcWatcher.setFuture(m_calculator->backend()->asyncCompute(stripped));
      return;
    }

    bool containsNonAlnum = std::ranges::any_of(query, [](QChar ch) { return !ch.isLetterOrNumber(); });
    auto isAllowedLeadingChar = [](QChar c) { return c == '(' || c == ')' || c.isLetterOrNumber(); };
    bool isComputable = expression.size() > 1 && isAllowedLeadingChar(expression.at(0)) && containsNonAlnum;

    if (!isComputable || !m_calculator->backend()) { return; }

    m_calcWatcher.setFuture(m_calculator->backend()->asyncCompute(expression));
  }

  void handleCalculatorFinished() {
    if (!m_calcWatcher.isFinished() || m_calculatorSearchQuery != query) return;
    if (auto res = m_calcWatcher.result()) {
      m_calc = res.value();
      emit dataChanged();
    }
  }

  void startFileSearch() {
    if (m_isFileSearchEnabled && query.size() >= MIN_FS_TEXT_LENGTH) {
      if (m_fileWatcher.isRunning()) { m_fileWatcher.cancel(); }
      m_fileSearchQuery = query;
      m_fileWatcher.setFuture(m_fs->queryAsync(query));
    }
  }

  void handleFileSearchFinished() {
    if (!m_fileWatcher.isFinished() || m_fileSearchQuery != query) return;
    m_files = m_fileWatcher.result();
    m_fileSearchQuery.clear();
    emit dataChanged();
  }

  std::string query;
  std::span<RootItemManager::ScoredItem> m_items;
  std::optional<AbstractCalculatorBackend::CalculatorResult> m_calc;

  QFutureWatcher<AbstractCalculatorBackend::ComputeResult> m_calcWatcher; // async calculator invocation
  FileSearchWatcher m_fileWatcher;                                        // async file search computation

  std::vector<IndexerFileResult> m_files;
  std::vector<std::shared_ptr<RootItem>> m_fallbackItems;

  // without query only
  std::vector<RootItemManager::SearchableRootItem> m_favorites;

  std::optional<LinkItem> m_defaultOpener;

  std::string m_fallbackSectionTitle;
  std::string m_resultSectionTitle;

  bool m_isFileSearchEnabled = false;

  // DI
  RootItemManager *m_manager = nullptr;
  FileService *m_fs = nullptr;
  AppService *m_appDb = nullptr;
  CalculatorService *m_calculator = nullptr;

  QTimer m_fileSearchDebounce;
  QTimer m_calculatorDebounce;

  std::string m_fileSearchQuery;
  std::string m_calculatorSearchQuery;
};

class RootSearchView2 : public BaseView {
public:
  RootSearchView2() { VStack().add(m_list).imbue(this); }

  QString initialSearchPlaceholderText() const override { return "Search for anything..."; }

  void handleSelection(std::optional<vicinae::ui::VListModel::Index> idx) {
    auto &nav = context()->navigation;

    if (!idx) {
      nav->destroyCurrentCompletion();
      clearActions();
      return;
    }

    if (auto data = m_model->fromIndex(idx.value())) {
      setActions(m_model->createActionPanel(data.value(), context()));
      if (auto completer = m_model->createCompleter(data.value())) {
        nav->createCompletion(completer->arguments, completer->iconUrl);
      } else {
        nav->destroyCurrentCompletion();
      }
    } else {
      qDebug() << "no data";
    }
  }

  bool tryAliasFastTrack() {
    auto selection = m_list->currentSelection();

    if (!selection) return false;

    if (m_model->shouldFastTrack(m_model->fromIndex(*selection).value())) {
      executePrimaryAction();
      return true;
    }

    return false;
  }

  bool inputFilter(QKeyEvent *event) override {
    auto config = ServiceRegistry::instance()->config();
    const QString &keybinding = config->value().keybinding;

    if (event->modifiers() == Qt::ControlModifier) {
      if (KeyBindingService::isDownKey(event, keybinding)) { return m_list->selectDown(); }
      if (KeyBindingService::isUpKey(event, keybinding)) { return m_list->selectUp(); }
      if (KeyBindingService::isLeftKey(event, keybinding)) {
        context()->navigation->popCurrentView();
        return true;
      }
      if (KeyBindingService::isRightKey(event, keybinding)) {
        m_list->activateCurrentSelection();
        return true;
      }
    }

    if (event->modifiers().toInt() == 0) {
      switch (event->key()) {
      case Qt::Key_Space:
        return tryAliasFastTrack();
      case Qt::Key_Up:
        return m_list->selectUp();
        break;
      case Qt::Key_Down:
        return m_list->selectDown();
        break;
      case Qt::Key_Tab: {
        if (!context()->navigation->hasCompleter()) {
          // m_list->selectNext();
          return true;
        }
        break;
      }
      case Qt::Key_Return:
      case Qt::Key_Enter:
        m_list->activateCurrentSelection();
        return true;
      }
    }

    return BaseView::inputFilter(event);
  }

  void initialize() override {
    auto config = context()->services->config();
    m_manager = context()->services->rootItemManager();
    m_model = new RootSearchModel(m_manager, context()->services->fileService(), context()->services->appDb(),
                                  context()->services->calculatorService());
    m_list->setModel(m_model);
    m_model->setFilter("");

    connect(m_list, &vicinae::ui::VListWidget::itemSelected, this, &RootSearchView2::handleSelection);
    connect(m_list, &vicinae::ui::VListWidget::itemActivated, this, [this]() { executePrimaryAction(); });
    connect(config, &ConfigService::configChanged, this,
            [&](const ConfigService::Value &next, const ConfigService::Value &prev) {
              m_model->setFileSearch(next.rootSearch.searchFiles);
            });
  }

  void textChanged(const QString &text) override {
    m_model->setFilter(text.simplified().toStdString());
    m_list->selectFirst();
  }

private:
  RootItemManager *m_manager = nullptr;
  RootSearchModel *m_model = nullptr;
  vicinae::ui::VListWidget *m_list = new vicinae::ui::VListWidget;
};
