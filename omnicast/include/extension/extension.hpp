#pragma once
#include "common.hpp"
#include "omni-icon.hpp"
#include "preference.hpp"
#include <filesystem>
#include <qjsonobject.h>
#include <qstring.h>
#include <filesystem>
#include <qstringview.h>
#include <qjsonarray.h>

class Extension : public AbstractCommandRepository {
private:
  QString m_id;
  QString m_name;
  QString m_title;
  QString m_icon;
  QString m_description;
  std::filesystem::path m_path;
  PreferenceList m_preferences;
  std::vector<std::shared_ptr<AbstractCmd>> m_commands;

  explicit Extension(const QJsonObject &object);

public:
  Extension() {}

  static Extension fromObject(const QJsonObject &object);

  static CommandArgument parseArgumentFromObject(const QJsonObject &obj);
  static Preference parsePreferenceFromObject(const QJsonObject &obj);

  QString id() const override;
  QString name() const override;
  OmniIconUrl iconUrl() const override;
  QString description() const override;
  std::filesystem::path assetDirectory() const;
  std::filesystem::path installedPath() const;
  PreferenceList preferences() const override;
  std::vector<std::shared_ptr<AbstractCmd>> commands() const override;
};
