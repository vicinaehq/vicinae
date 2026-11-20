#include <catch2/catch_test_macros.hpp>
#include <QFile>
#include <QTemporaryDir>
#include <filesystem>
#include <glaze/json/read.hpp>
#include "services/extension-registry/extension-manifest.hpp"

namespace fs = std::filesystem;

namespace {

fs::path writeExtension(const QTemporaryDir &dir, const char *id, const char *json) {
  const auto root = fs::path(dir.path().toStdString()) / id;
  fs::create_directories(root);

  QFile file(root / "package.json");
  REQUIRE(file.open(QIODevice::WriteOnly));
  REQUIRE(file.write(json) > 0);

  return root;
}

constexpr auto MANIFEST = R"({
  "$schema": "https://www.raycast.com/schemas/extension.json",
  "name": "todo",
  "title": "Todo",
  "description": "Manage todos",
  "icon": "icon.png",
  "author": "someone",
  "contributors": ["other"],
  "categories": ["Productivity"],
  "preferences": [
    { "name": "token", "type": "password", "title": "Token", "required": true },
    { "name": "compact", "type": "checkbox", "title": "Compact", "label": "Use compact mode", "default": true },
    { "name": "sort", "type": "dropdown", "title": "Sort", "default": "date",
      "data": [{ "title": "Date", "value": "date" }, { "title": "Name", "value": "name" }] }
  ],
  "commands": [
    { "name": "list", "title": "List Todos", "description": "List", "mode": "view", "keywords": ["tasks"],
      "arguments": [
        { "name": "query", "type": "text", "placeholder": "Query", "required": true },
        { "name": "status", "type": "dropdown", "placeholder": "Status",
          "data": [{ "title": "Open", "value": "open" }] }
      ] },
    { "name": "sync", "title": "Sync", "mode": "no-view", "interval": "10m", "disabledByDefault": true },
    { "name": "bar", "title": "Bar", "mode": "menu-bar" }
  ],
  "tools": [
    { "name": "create-todo", "title": "Create Todo", "description": "Create a todo",
      "instructions": "Creates a new todo",
      "input": {
        "type": "object",
        "properties": { "title": { "type": "string" } },
        "required": ["title"]
      },
      "output": { "type": "string" },
      "confirmation": true },
    { "name": "list-todos", "title": "List Todos", "description": "List todos", "confirmation": false },
    { "name": "mention", "title": "Mention", "description": "Mention", "functionalities": ["AI mention search"] }
  ],
  "dependencies": { "@raycast/api": "^1.90.0" }
})";

} // namespace

// Catch2 assertions guard optional access in ways the checker can't follow
// NOLINTBEGIN(bugprone-throwing-static-initialization, bugprone-unchecked-optional-access)
TEST_CASE("extension manifest parses extension metadata") {
  const QTemporaryDir dir;
  REQUIRE(dir.isValid());
  const auto root = writeExtension(dir, "store.raycast.todo", MANIFEST);

  const auto manifest = ExtensionManifest::fromPackageJson(root);
  REQUIRE(manifest.has_value());

  CHECK(manifest->id == "store.raycast.todo");
  CHECK(manifest->name == "todo");
  CHECK(manifest->title == "Todo");
  CHECK(manifest->icon == "icon.png");
  CHECK(manifest->author == "someone");
  CHECK(manifest->categories == std::vector<QString>{"Productivity"});
  CHECK(manifest->needsRaycastApi);
  CHECK(manifest->isFromRaycastStore());
}

TEST_CASE("extension manifest parses preferences") {
  const QTemporaryDir dir;
  REQUIRE(dir.isValid());
  const auto manifest = ExtensionManifest::fromPackageJson(writeExtension(dir, "todo", MANIFEST));
  REQUIRE(manifest.has_value());
  CHECK(manifest->isLocal());

  const auto &prefs = manifest->preferences;
  REQUIRE(prefs.size() == 3);

  CHECK(prefs.at(0).name() == "token");
  CHECK(prefs.at(0).required());
  CHECK(std::holds_alternative<Preference::PasswordData>(prefs.at(0).data()));

  const auto checkboxData = prefs.at(1).data();
  const auto *checkbox = std::get_if<Preference::CheckboxData>(&checkboxData);
  REQUIRE(checkbox);
  CHECK(checkbox->label == "Use compact mode");
  REQUIRE(prefs.at(1).defaultValue().has_value());
  CHECK(prefs.at(1).defaultValue().value().get<bool>());

  const auto dropdownData = prefs.at(2).data();
  const auto *dropdown = std::get_if<Preference::DropdownData>(&dropdownData);
  REQUIRE(dropdown);
  REQUIRE(dropdown->options.size() == 2);
  CHECK(dropdown->options.at(1).title == "Name");
  CHECK(dropdown->options.at(1).value == "name");
  REQUIRE(prefs.at(2).defaultValue().has_value());
  CHECK(prefs.at(2).defaultValue().value().get<std::string>() == "date");
}

TEST_CASE("extension manifest parses supported commands") {
  const QTemporaryDir dir;
  REQUIRE(dir.isValid());
  const auto root = writeExtension(dir, "todo", MANIFEST);
  const auto manifest = ExtensionManifest::fromPackageJson(root);
  REQUIRE(manifest.has_value());

  REQUIRE(manifest->commands.size() == 2);

  const auto &list = manifest->commands.at(0);
  CHECK(list.name == "list");
  CHECK(list.mode == CommandModeView);
  CHECK(list.entrypoint == root / "list.js");
  CHECK(list.keywords == std::vector<QString>{"tasks"});
  CHECK_FALSE(list.interval.has_value());
  CHECK_FALSE(list.defaultDisabled);

  REQUIRE(list.arguments.size() == 2);
  CHECK(list.arguments.at(0).type == CommandArgument::Text);
  CHECK(list.arguments.at(0).required);
  CHECK(list.arguments.at(1).type == CommandArgument::Dropdown);
  REQUIRE(list.arguments.at(1).data.has_value());
  CHECK(list.arguments.at(1).data.value().at(0).value == "open");

  const auto &sync = manifest->commands.at(1);
  CHECK(sync.mode == CommandModeNoView);
  CHECK(sync.interval == std::chrono::minutes(10));
  CHECK(sync.defaultDisabled);
}

TEST_CASE("extension manifest parses AI tools") {
  const QTemporaryDir dir;
  REQUIRE(dir.isValid());
  const auto root = writeExtension(dir, "todo", MANIFEST);
  const auto manifest = ExtensionManifest::fromPackageJson(root);
  REQUIRE(manifest.has_value());

  REQUIRE(manifest->tools.size() == 2);

  const auto &create = manifest->tools.at(0);
  CHECK(create.name == "create-todo");
  CHECK(create.title == "Create Todo");
  CHECK(create.instructions == std::optional<std::string>{"Creates a new todo"});
  CHECK(create.confirmation);
  CHECK(create.entrypoint == root / "tools" / "create-todo.js");
  REQUIRE(create.inputSchema.has_value());

  // tool schemas end up in newline-delimited protocols such as MCP over stdio
  CHECK_FALSE(create.inputSchema->contains('\n'));

  glz::generic schema;
  REQUIRE_FALSE(glz::read_json(schema, create.inputSchema.value()));
  CHECK(schema.at("type").get<std::string>() == "object");
  CHECK(schema.at("properties").at("title").at("type").get<std::string>() == "string");

  const auto &list = manifest->tools.at(1);
  CHECK_FALSE(list.inputSchema.has_value());
  CHECK_FALSE(list.instructions.has_value());
  CHECK_FALSE(list.confirmation);
}

TEST_CASE("extension manifest reports missing and malformed package.json") {
  const QTemporaryDir dir;
  REQUIRE(dir.isValid());

  CHECK_FALSE(ExtensionManifest::fromPackageJson(fs::path(dir.path().toStdString()) / "missing").has_value());
  CHECK_FALSE(ExtensionManifest::fromPackageJson(writeExtension(dir, "broken", "{ \"name\": ")).has_value());
  CHECK_FALSE(
      ExtensionManifest::fromPackageJson(writeExtension(dir, "wrong-type", R"({ "commands": "nope" })"))
          .has_value());
}
// NOLINTEND(bugprone-throwing-static-initialization, bugprone-unchecked-optional-access)
