#pragma once
#include "cli.hpp"
#include "codegen/codegen.hpp"
#include "codegen/typescript.hpp"
#include "codegen/glaze-qt.hpp"
#include "codegen/glaze.hpp"
#include "parser.hpp"
#include <fstream>

class CompileCommand : public AbstractCommandLineCommand {
public:
  std::string id() const override { return "compile"; }

  void setup(CLI::App *app) override {
    app->add_option("proto", m_proto)->required();
    app->add_option("--output,-o", m_out, "")->required();
    app->add_option("--client", m_clients, "");
    app->add_option("--server", m_servers, "");
    app->add_option("--namespace", m_opts.generationNamespace);
  }

  bool run(CLI::App *app) override {
    std::ifstream ifs{m_proto};

    if (!ifs) {
      std::cerr << "could not read" << m_proto << std::endl;
      return false;
    }

    std::ostringstream oss;
    oss << ifs.rdbuf();
    std::string data = oss.str();

    auto result = Parser::parseTree(data);

    if (!result) {
      std::cerr << "Failed to parse " << m_proto << "\n" << result.error() << std::endl;
      return false;
    }

    auto tree = std::move(result).value();

    std::vector<std::unique_ptr<AbstractCodeGenerator>> generators;
    generators.emplace_back(std::make_unique<TypeScriptCodeGenerator>());
    generators.emplace_back(std::make_unique<GlazeQtGenerator>());
    generators.emplace_back(std::make_unique<GlazeGenerator>());

    for (const auto &client : m_clients) {
      for (const auto &gen : generators) {
        if (gen->name() == client) {
          CodegenOptions opts = m_opts;
          opts.file = m_proto;

          std::ofstream{m_out} << gen->generateClient(tree, opts);
          std::cout << "generated " << client << " client at " << m_out << std::endl;
          return true;
        }
      }
    }

    for (const auto &server : m_servers) {
      for (const auto &gen : generators) {
        if (gen->name() == server) {
          CodegenOptions opts = m_opts;
          opts.file = m_proto;
          std::ofstream{m_out} << gen->generateServer(tree, opts);
          std::cout << "generated " << server << " server at " << m_out << std::endl;
          return true;
        }
      }
    }

    return true;
  }

private:
  std::string m_proto;
  std::string m_out;
  CodegenOptions m_opts;
  std::vector<std::string> m_clients;
  std::vector<std::string> m_servers;
};
