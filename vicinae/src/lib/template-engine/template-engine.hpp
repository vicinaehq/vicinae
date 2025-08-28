#pragma once
#include <qstring.h>
#include <unordered_map>
#include <qhash.h>

/**
 * Utility class to build template strings.
 */
class TemplateEngine {
public:
  void setVar(const QString &identifier, const QString &value);
  std::optional<QString> var(const QString &key) const;

  QString build(const QString &schema);

private:
  QString m_schema;
  std::unordered_map<QString, QString> m_vars;
};
