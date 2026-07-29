#include "fs.hpp"
#include <array>
#include <glaze/glaze.hpp>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include "ipc-client.hpp"

namespace {

static constexpr std::array FILE_CATEGORY_NAMES = {
    "image", "video", "audio", "document", "archive", "application", "directory", "other",
};

std::vector<std::string> fileCategoryNames() {
  std::vector<std::string> names;

  names.reserve(FILE_CATEGORY_NAMES.size());
  for (std::string_view name : FILE_CATEGORY_NAMES) {
    names.emplace_back(name);
  }

  return names;
}

std::string fileCategoryDescription() {
  std::string out = "filter by file category: ";

  for (size_t idx = 0; idx != FILE_CATEGORY_NAMES.size(); ++idx) {
    if (idx != 0) out += ", ";
    out += FILE_CATEGORY_NAMES[idx];
  }

  return out;
}

} // namespace

class FileQueryCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "query"; }
  std::string description() const override {
    return "Return a list of indexed files matching the given query";
  }

  void setup(CLI::App *app) override {
    app->alias("q");
    app->add_option("query", m_query, "fuzzyish search query")->required();
    app->add_option("-n,--limit", m_limit, "limit the number of results (defaults to 100, up to 10,000)")
        ->default_val(100);
    app->add_option("-c,--category", m_category, fileCategoryDescription())
        ->check(CLI::IsMember(fileCategoryNames()));
    app->add_flag("-j,--json", json, "output result set as json")->default_val(false);
  }

  bool run(CLI::App *) override {
    auto client = cli::IpcClient::connect();

    if (!client) throw std::runtime_error(client.error());

    if (m_query.size() < 3) { throw std::runtime_error("Query should be at least 3 characters long"); }

    auto results = client->fsQuery(m_query, m_limit, m_category).value();

    if (json) {
      std::string buf;
      std::ignore = glz::write_json(results, buf);
      std::cout << glz::prettify_json(buf);
    } else {
      for (const auto &result : results) {
        std::cout << result.path << "\n";
      }
    }

    return true;
  }

  int m_limit;
  std::string m_query;
  std::optional<std::string> m_category;
  bool json = false;
};

FileSearchCommand::FileSearchCommand() { registerCommand<FileQueryCommand>(); }
