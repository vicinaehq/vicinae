#include <array>
#include <fstream>
#include <stack>
#include "script-scanner.hpp"
#include "script-command-file.hpp"
#include "utils.hpp"

static bool isProbablyTextFile(const std::filesystem::path &path) {
  std::ifstream ifs(path, std::ios::binary);

  if (!ifs) return false;

  std::array<char, 8192> buf;

  ifs.read(buf.data(), buf.size());

  const std::string_view head(buf.data(), static_cast<size_t>(ifs.gcount()));

  return !head.contains('\0');
}

std::vector<std::shared_ptr<ScriptCommandFile>>
ScriptScanner::scan(std::span<const std::filesystem::path> dirs) {
  static constexpr const std::uint8_t MAX_DEPTH = 5;
  std::vector<std::shared_ptr<ScriptCommandFile>> scripts;
  std::error_code ec;
  std::stack<ScannedDirectory> dirStack;
  std::unordered_set<std::string> idsSeen;

  static const auto forbiddenExtensions = {".md", ".svg", ".txt"};

  for (const auto &dir : dirs) {
    dirStack.emplace(ScannedDirectory("", dir));
  }

  while (!dirStack.empty()) {
    const ScannedDirectory dir = std::move(dirStack.top());
    dirStack.pop();

    for (const auto &ent : std::filesystem::directory_iterator(dir.path, ec)) {
      std::string const filename = getLastPathComponent(ent.path());

      if (filename.starts_with('.') || filename.contains(".template")) continue;

      std::string id = dir.id;

      if (!id.empty()) id += '.';

      id += filename;

      if (ent.is_directory() && dir.depth + 1 < MAX_DEPTH) {
        dirStack.emplace(ScannedDirectory(id, ent.path(), dir.depth + 1));
        continue;
      }

      if (idsSeen.contains(id)) continue;

      const auto ext = ent.path().extension();

      if (std::ranges::contains(forbiddenExtensions, ext)) { continue; }

      if (!isProbablyTextFile(ent.path())) continue;

      auto script = ScriptCommandFile::fromFile(ent.path(), id);

      if (!script) {
        qDebug() << "Failed to parse script at" << ent.path().c_str() << script.error().c_str();
        continue;
      }

      scripts.emplace_back(std::make_shared<ScriptCommandFile>(script.value()));
      idsSeen.insert(id);
    }
  }

  return scripts;
}
