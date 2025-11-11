#include "services/extension-boilerplate-generator/extension-boilerplate-generator.hpp"
#include "common.hpp"
#include "utils/environment.hpp"
#include "utils/utils.hpp"
#include <filesystem>
#include <qfile.h>

namespace fs = std::filesystem;

#define PLACEHOLDER(x) "%" x "%"

static const QString COMMAND_JSON_TEMPLATE = R"(    {
      "name": "%NAME%",
      "title": "%TITLE%",
      "subtitle": "%SUBTITLE%",
      "description": "%DESCRIPTION%",
      "mode": "%MODE%"
    })";

static const QString COMMAND_LIST_JSON_TEMPLATE = "[\n%1\n  ]";

static const std::vector<CommandBoilerplate> CMD_TEMPLATE_LIST = {
    CommandBoilerplate{.resource = ":boilerplate/tmpl-list", .name = "Simple List", .mode = CommandModeView},
    CommandBoilerplate{
        .resource = ":boilerplate/tmpl-list-detail", .name = "List with Detail", .mode = CommandModeView},
    CommandBoilerplate{
        .resource = ":boilerplate/tmpl-controlled-list", .name = "Controlled List", .mode = CommandModeView},
    CommandBoilerplate{
        .resource = ":boilerplate/tmpl-simple-detail", .name = "Simple Detail", .mode = CommandModeView},
    CommandBoilerplate{.resource = ":boilerplate/tmpl-no-view", .name = "No View", .mode = CommandModeNoView},
};

const std::vector<CommandBoilerplate> &ExtensionBoilerplateGenerator::commandBoilerplates() const {
  return CMD_TEMPLATE_LIST;
}

tl::expected<fs::path, QString>
ExtensionBoilerplateGenerator::generate(const fs::path &targetDir, const ExtensionBoilerplateConfig &config) {
  std::error_code ec;
  QString extName = slugify(config.title);

  auto userCopy = [](const QString &src, const QString &dst) {
    QFile::copy(src, dst);
    QFile::setPermissions(dst, QFileDevice::ReadOwner | QFileDevice::WriteOwner);
  };

  if (!fs::is_directory(targetDir, ec)) {
    return tl::unexpected(QString("%1 is not a directory. The boilerplate generator will not create the "
                                  "containing directory for you")
                              .arg(targetDir.c_str()));
  }

  fs::path extDir = targetDir / extName.toStdString();

  if (fs::exists(extDir, ec)) {
    return tl::unexpected(QString("%1 already exists. Won't override.").arg(extDir.c_str()));
  }

  fs::path srcDir = extDir / "src";
  fs::path assetsDir = extDir / "assets";
  fs::create_directories(srcDir);
  fs::create_directories(assetsDir);

  QFile packageJson(":boilerplate/package.json");

  if (!packageJson.open(QIODevice::ReadOnly)) {
    return tl::unexpected("Cannot open package.json boilerplate");
  }

  QString manifest = packageJson.readAll();
  QStringList cmdStrings;

  for (const auto &cmd : config.commands) {

    QString name = slugify(cmd.title);

    auto pred = [&](auto &&tmpl) { return tmpl.resource == cmd.templateId; };
    auto it = std::ranges::find_if(CMD_TEMPLATE_LIST, pred);

    if (it == CMD_TEMPLATE_LIST.end()) {
      return tl::unexpected(QString("Unknown template with id %1").arg(cmd.templateId));
    }

    QString mode = it->mode == CommandModeView ? "view" : "no-view";
    QString cmdString = QString(COMMAND_JSON_TEMPLATE)
                            .replace(PLACEHOLDER("NAME"), name.simplified())
                            .replace(PLACEHOLDER("TITLE"), cmd.title.simplified())
                            .replace(PLACEHOLDER("SUBTITLE"), cmd.subtitle.simplified())
                            .replace(PLACEHOLDER("DESCRIPTION"), cmd.description.simplified())
                            .replace(PLACEHOLDER("MODE"), mode);

    QString ext = it->mode == CommandModeView ? "tsx" : "ts";
    QString filename = QString("%1.%2").arg(name).arg(ext);

    userCopy(it->resource, QString::fromStdString(srcDir / filename.toStdString()));
    cmdStrings << cmdString;
  }

  QString version = Environment::version();
  bool isValidVersion = version.startsWith('v') && version.sliced(1).split('.').size() == 3;

  if (isValidVersion) {
    version[0] = '^';
  } else {
    version = "^0.8.0"; // TODO: all builds should have a valid version, but this is provided as a quick fix
  }

  // we don't use QJson because key order would not be preserved
  manifest.replace(PLACEHOLDER("NAME"), extName)
      .replace(PLACEHOLDER("TITLE"), config.title.simplified())
      .replace(PLACEHOLDER("DESCRIPTION"), config.description.simplified())
      .replace(PLACEHOLDER("AUTHOR"), config.author.simplified())
      .replace(PLACEHOLDER("VICINAE_VERSION"), version)
      .replace(PLACEHOLDER("COMMAND_LIST"), COMMAND_LIST_JSON_TEMPLATE.arg(cmdStrings.join(",\n    ")));

  {
    QFile file(extDir / "package.json");

    if (!file.open(QIODevice::WriteOnly)) { return tl::unexpected(QString("Failed to write manifest")); }

    file.write(manifest.toUtf8());
  }

  QFile gitignore(extDir / ".gitignore");

  if (!gitignore.open(QIODevice::WriteOnly)) { return tl::unexpected(QString("Failed to write gitignore")); }

  gitignore.write("node_modules\nvicinae-env.d.ts\n");
  userCopy(":boilerplate/tsconfig.json", QString::fromStdString(extDir / "tsconfig.json"));
  userCopy(":boilerplate/extension_icon", QString::fromStdString(assetsDir / "extension_icon.png"));
  userCopy(":boilerplate/README.md", QString::fromStdString(extDir / "README.md"));

  return extDir;
}

ExtensionBoilerplateGenerator::ExtensionBoilerplateGenerator() {}
