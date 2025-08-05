#include "ui/qtheme-selector/qtheme-selector.hpp"
#include <filesystem>
#include <ranges>
#include <QIcon>
#include <system_error>
#include "qtheme-selector.hpp"
#include "utils/utils.hpp"

namespace fs = std::filesystem;

QThemeSelector::QThemeSelector() {
  std::set<QString> themeNames;

  for (const auto &s : QIcon::themeSearchPaths()) {
    fs::path path(s.toStdString());
    std::error_code ec;

    for (const auto &entry : fs::directory_iterator(path, ec)) {
      if (!entry.is_directory()) continue;

      themeNames.insert(getLastPathComponent(entry).c_str());
    }
  }

  auto items = themeNames |
               std::views::transform([](auto &&path) -> std::shared_ptr<SelectorInput::AbstractItem> {
                 return std::make_shared<QThemeSelectorItem>(path);
               }) |
               std::ranges::to<std::vector>();

  addSection("", items);
  updateModel();
}
