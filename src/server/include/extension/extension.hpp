#pragma once
#include "command.hpp"
#include "services/extension-registry/extension-manifest.hpp"
#include "ui/image/url.hpp"
#include "preference.hpp"
#include <filesystem>
#include <qjsonobject.h>
#include <qstring.h>
#include <filesystem>
#include <qstringview.h>
#include <qjsonarray.h>

class Extension : public AbstractCommandRepository {
private:
  std::vector<std::shared_ptr<AbstractCmd>> m_commands;
  ExtensionManifest m_manifest;

public:
  Extension() {}
  Extension(const ExtensionManifest &manifest);

  QString id() const override;
  QString displayName() const override;
  QString name() const override;
  ImageURL iconUrl() const override;
  QString description() const override;
  std::filesystem::path assetDirectory() const;
  std::filesystem::path installedPath() const;
  PreferenceList preferences() const override;
  std::vector<std::shared_ptr<AbstractCmd>> commands() const override;
  QString author() const override;
  const ExtensionManifest &manifest() const;
};
