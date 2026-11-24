#include <ranges>
#include "extension/extension.hpp"
#include "extension/extension-command.hpp"
#include "extension/extension-tool.hpp"
#include "ui/image/url.hpp"
#include "command/preference.hpp"
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qlogging.h>

ImageURL Extension::iconUrl() const {
  auto fallback = ImageURL::builtin(BuiltinIcon::Hammer).setBackgroundTint(SemanticColor::Cyan);

  if (!m_manifest.icon.isEmpty()) {
    return ImageURL::local(assetDirectory() / m_manifest.icon.toStdString()).withFallback(fallback);
  }

  return fallback;
}

QString Extension::id() const { return QString("@%1/%2").arg(m_manifest.author).arg(m_manifest.id); }

QString Extension::displayName() const { return m_manifest.title; }

QString Extension::name() const { return m_manifest.name; }

std::filesystem::path Extension::assetDirectory() const { return m_manifest.path / "assets"; }

std::filesystem::path Extension::installedPath() const { return m_manifest.path; }

PreferenceList Extension::preferences() const { return m_manifest.preferences; }

QString Extension::author() const { return m_manifest.author; }

std::vector<std::shared_ptr<AbstractCmd>> Extension::commands() const { return m_commands; }

QString Extension::description() const { return m_manifest.description; };

const ExtensionManifest &Extension::manifest() const { return m_manifest; }

void Extension::configureCommand(ExtensionCommand &command) const {
  command.setExtensionId(m_manifest.id);
  command.setPath(m_manifest.path);
  command.setExtensionTitle(m_manifest.title);
  command.setExtensionIcon(m_manifest.icon);
  command.setExtensionPreferences(m_manifest.preferences);
  command.setExtensionName(m_manifest.name);
  command.setAuthor(author());
}

Extension::Extension(const ExtensionManifest &manifest) : m_manifest(manifest) {
  m_commands.reserve(m_manifest.commands.size());
  for (const auto &cmd : m_manifest.commands) {
    auto command = std::make_shared<ExtensionCommand>(cmd);
    configureCommand(*command);
    m_commands.emplace_back(std::move(command));
  }
}

QString Extension::toolNamespace() const { return m_manifest.name; }

std::shared_ptr<ExtensionCommand> Extension::makeToolCommand(const ExtensionManifest::Tool &tool) const {
  auto command = std::make_shared<ExtensionToolCommand>(ExtensionManifest::Command{
      .name = QString::fromStdString(tool.name),
      .title = tool.title,
      .description = tool.description,
      .mode = CommandModeNoView,
      .preferences = tool.preferences,
      .entrypoint = tool.entrypoint,
      .defaultDisabled = false,
      .provenance = m_manifest.provenance,
  });
  configureCommand(*command);
  return command;
}

std::vector<AI::ToolContribution> Extension::tools() const {
  const auto toContribution = [this](const ExtensionManifest::Tool &tool) {
    auto icon = iconUrl();
    if (tool.icon) icon = ImageURL::local(assetDirectory() / tool.icon->toStdString()).withFallback(icon);

    return AI::ToolContribution{
        .id = tool.name,
        .title = tool.title,
        .description = tool.description,
        .icon = std::move(icon),
        .preferences = tool.preferences,
        .annotations = tool.confirmation ? AI::ToolAnnotations{.readOnly = false, .destructive = true}
                                         : AI::ToolAnnotations{},
        .create = [tool, command = makeToolCommand(tool)] -> std::unique_ptr<AI::AbstractTool> {
          return std::make_unique<ExtensionTool>(tool, command);
        },
    };
  };

  return m_manifest.tools | std::views::transform(toContribution) | std::ranges::to<std::vector>();
}
