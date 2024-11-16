#include <cctype>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
namespace Ini {
struct Iterator {
  std::string_view data;

  Iterator(std::string_view data) : data(data) {}
};

class Parser {
public:
  std::optional<
      std::function<void(std::string_view, std::string_view, std::string_view)>>
      keyCb = std::nullopt;
  std::optional<std::function<void(std::string_view)>> sectionCb = std::nullopt;
  std::ifstream &ifs;

  Parser(std::ifstream &ifs) : ifs(ifs) {}

  void onSection(std::function<void(std::string_view)> cb) { sectionCb = cb; }

  void onKey(
      std::function<void(std::string_view, std::string_view, std::string_view)>
          cb) {
    keyCb = cb;
  }

  void parse() {
    std::string line;
    std::string section;

    while (std::getline(ifs, line)) {
      size_t i = 0;

      while (i < line.size() && std::isblank(line.at(i)))
        ++i;

      // empty line
      if (i == line.size() || line.at(i) == '#')
        continue;

      if (line.at(i) == '[') {
        size_t j = ++i;

        while (j < line.size() && line.at(j) != ']')
          ++j;

        // invalid section declaration, ignore
        if (j == line.size() || line.at(j) != ']')
          continue;

        section = {line.begin() + i, line.begin() + j};
        if (sectionCb)
          (*sectionCb)(section);
        continue;
      }

      size_t j = i;

      while (j < line.size() && line.at(j) != '=')
        ++j;

      std::string_view key(line.begin() + i, line.begin() + j);

      i = ++j;

      std::string_view value(line.begin() + i, line.begin() + line.size());

      if (keyCb)
        (*keyCb)(section, key, value);

      // parse k v
    }
  }
};
} // namespace Ini
