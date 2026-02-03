#pragma once
#include <QString>
#include <map>
#include <span>

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
  };

  using UrlPart = std::variant<QString, ParsedPlaceholder>;

  /**
   * Parse a string composed of placeholders in order to make it easy to know what positional arguments are
   * required. Positional arguments can be explicitly argument placeholders such as "{argument name="XXX"}"
   * but can also be any placeholder that is not reserved "{my_query}".
   */
  static PlaceholderString parse(QString link, std::span<const QString> reserved);

  /**
   * Parse placeholder with placeholder names that are reserved in a snippet context.
   */
  static PlaceholderString parseSnippetText(QString link);

  /**
   * Parse placeholder with placeholder names that are reserved in a shortcut context.
   */
  static PlaceholderString parseShortcutText(QString link);

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
