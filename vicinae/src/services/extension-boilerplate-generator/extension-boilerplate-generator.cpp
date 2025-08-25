#include "services/extension-boilerplate-generator/extension-boilerplate-generator.hpp"
#include "common.hpp"
#include "utils/utils.hpp"
#include <absl/strings/internal/str_format/extension.h>
#include <filesystem>
#include <qfile.h>
#include <qjsondocument.h>
#include <qjsonparseerror.h>
#include <qjsonobject.h>
#include <qjsonarray.h>

static const std::vector<CommandBoilerplate> CMD_TEMPLATE_LIST = {
    CommandBoilerplate{
        .resource = ":boilerplate/tmpl-simple-detail", .name = "Simple Detail", .mode = CommandModeView},
    CommandBoilerplate{.resource = ":boilerplate/tmpl-no-view", .name = "No View", .mode = CommandModeNoView},
};

namespace fs = std::filesystem;

const std::vector<CommandBoilerplate> &ExtensionBoilerplateGenerator::commandBoilerplates() const {
  return CMD_TEMPLATE_LIST;
}

std::expected<void, QString>
ExtensionBoilerplateGenerator::generate(const fs::path &targetDir, const ExtensionBoilerplateConfig &config) {
  std::error_code ec;
  QString extName = slugify(config.title);

  auto userCopy = [](const QString &src, const QString &dst) {
    QFile::copy(src, dst);
    QFile::setPermissions(dst, QFileDevice::ReadOwner | QFileDevice::WriteOwner);
  };

  if (!fs::is_directory(targetDir, ec)) {
    return std::unexpected(QString("%1 is not a directory. The boilerplate generator will not create the "
                                   "containing directory for you")
                               .arg(targetDir.c_str()));
  }

  fs::path extDir = targetDir / extName.toStdString();
  fs::path srcDir = extDir / "src";
  fs::path assetsDir = extDir / "assets";
  fs::create_directories(srcDir);
  fs::create_directories(assetsDir);

  QFile packageJson(":boilerplate/package.json");

  if (!packageJson.open(QIODevice::ReadOnly)) {
    return std::unexpected("Cannot open package.json boilerplate");
  }

  QJsonParseError parseError;
  auto json = QJsonDocument::fromJson(packageJson.readAll(), &parseError);

  if (parseError.error != QJsonParseError::NoError) {
    return std::unexpected(QString("Failed to parse package.json: %1").arg(parseError.errorString()));
  }

  auto manifest = json.object();
  auto deps = manifest.value("dependencies").toObject();
  auto cmds = manifest.value("commands").toArray();
  QString version(VICINAE_GIT_TAG);

  if (!version.isEmpty()) version[0] = '^';

  deps["@vicinae/api"] = version;

  for (const auto &cmd : config.commands) {
    QJsonObject obj;
    QString name = slugify(cmd.title);

    obj["title"] = cmd.title;
    obj["name"] = name;
    obj["subtitle"] = cmd.subtitle;
    obj["description"] = cmd.description.simplified();

    auto pred = [&](auto &&tmpl) { return tmpl.resource == cmd.templateId; };
    auto it = std::ranges::find_if(CMD_TEMPLATE_LIST, pred);

    if (it == CMD_TEMPLATE_LIST.end()) {
      return std::unexpected(QString("Unknown template with id %1").arg(cmd.templateId));
    }

    // QString ext = it->mode == CommandModeView ? "tsx" : "ts";
    QString ext = "tsx";
    QString filename = QString("%1.%2").arg(name).arg(ext);

    obj["mode"] = it->mode == CommandModeView ? "view" : "no-view";
    userCopy(it->resource, QString::fromStdString(srcDir / filename.toStdString()));

    cmds.push_back(obj);
  }

  manifest["author"] = config.author;
  manifest["title"] = config.title;
  manifest["description"] = config.description;
  manifest["name"] = extName;
  manifest["commands"] = cmds;
  manifest["dependencies"] = deps;

  {
    QJsonDocument finalManifest;
    QFile file(extDir / "package.json");

    finalManifest.setObject(manifest);

    if (!file.open(QIODevice::WriteOnly)) {
      return std::unexpected(QString("Failed to parse package.json: %1").arg(parseError.errorString()));
    }

    file.write(finalManifest.toJson());
  }

  userCopy(":boilerplate/tsconfig.json", QString::fromStdString(extDir / "tsconfig.json"));
  userCopy(":boilerplate/extension_icon", QString::fromStdString(assetsDir / "extension_icon.png"));

  return {};
}

ExtensionBoilerplateGenerator::ExtensionBoilerplateGenerator() {}
