#include "trie.hpp"
#include <filesystem>
#include <iostream>
#include <string>

int main() {
  Trie<std::filesystem::path> trie;

  std::error_code ec;

  std::stack<std::filesystem::path> dirStack;

  dirStack.push("/home/aurelle/");

  size_t pathCharCount = 0;

  while (!dirStack.empty()) {
    auto path = dirStack.top();
    dirStack.pop();

    std::filesystem::directory_iterator it(path, ec);

    if (ec) continue;

    for (const auto &entry : it) {
      if (entry.is_symlink()) continue;

      auto str = entry.path().string();
      auto filename = entry.path().filename();

      if (filename.string().starts_with(".")) continue;
      if (filename.string() == "node_modules") continue;

      if (entry.is_directory()) { dirStack.push(entry.path()); }

      std::cout << "indexing " << filename << " for " << entry.path() << "\n";
      trie.index(filename.c_str(), entry.path());
      pathCharCount += entry.path().string().size();
    }
  }

  std::string line;

  std::cerr << "path char size " << pathCharCount / 1e6 << " MB";
  std::cerr << "file index> " << std::flush;

  while (std::getline(std::cin, line)) {
    std::cout << std::endl;

    if (!line.empty()) { trie.prefixSearch(line); }

    std::cerr << "file index> " << std::flush;
  }
}
