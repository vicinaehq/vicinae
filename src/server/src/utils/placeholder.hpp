#pragma once
#include <QString>
#include <QDateTime>
#include <map>
#include <optional>
#include <span>
#include <vector>

namespace placeholder_utils {
QDateTime applyDateOffset(QDateTime dt, QStringView offset);
QDateTime applyDateOffsets(QDateTime dt, QStringView offsetStr);
QString normalizeDateFormat(QString fmt);
QString applyModifier(QString val, QStringView mod);
} // namespace placeholder_utils

/**
 * A parsed placeholder string such as "Hi {first_name} {clipboard} {argument name="test"}
 */
class PlaceholderString {
public:
  struct Argument {
    QString name;
    QString defaultValue;
  };

  struct ParsedPlaceholder {
    QString id;
    std::map<QString, QString> args;
    std::vector<QString> modifiers;
    std::optional<QString> fallbackId;
  };

  using UrlPart = std::variant<QString, ParsedPlaceholder>;

  /**
   * Parse a string composed of placeholders in order to make it easy to know what positional arguments are
   * required. Positional arguments can be explicitly argument placeholders such as "{argument name="XXX"}"
   * but can also be any placeholder that is not reserved "{my_query}".
   */
  static PlaceholderString parse(const QString &link, std::span<const QString> reserved);

  /**
   * Parse placeholder with placeholder names that are reserved in a snippet context.
   */
  static PlaceholderString parseSnippetText(const QString &link);

  /**
   * Parse placeholder with placeholder names that are reserved in a shortcut context.
   */
  static PlaceholderString parseShortcutText(const QString &link);

  std::span<const Argument> arguments() const { return m_args; }
  std::span<const ParsedPlaceholder> placeholders() const { return m_placeholders; }
  std::span<const UrlPart> parts() const { return m_parts; }
  const QString &raw() const { return m_raw; }

private:
  std::vector<ParsedPlaceholder> m_placeholders;
  std::vector<Argument> m_args;
  std::vector<UrlPart> m_parts;
  QString m_raw;
};
