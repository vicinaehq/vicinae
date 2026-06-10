#include "fs.hpp"
#include <glaze/glaze.hpp>
#include <iostream>
#include <stdexcept>
#include "ipc-client.hpp"

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
    app->add_flag("-j,--json", json, "output result set as json")->default_val(false);
  }

  bool run(CLI::App *) override {
    auto client = cli::IpcClient::connect();

    if (!client) throw std::runtime_error(client.error());

    if (m_query.size() < 3) { throw std::runtime_error("Query should be at least 3 characters long"); }

    auto results = client->fsQuery(m_query, m_limit).value();

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
  bool json = false;
};

FileSearchCommand::FileSearchCommand() { registerCommand<FileQueryCommand>(); }
