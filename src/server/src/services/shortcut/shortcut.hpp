#pragma once
#include <qdatetime.h>
#include <qlogging.h>
#include <qdebug.h>
#include <qobject.h>
#include <qsqlquery.h>
#include <qstring.h>
#include <qsqlerror.h>
#include <qtmetamacros.h>

class Shortcut {
  /**
   * A list of reserved placeholder IDs. Each ID is meant to implement its own expansion rules.
   * A placeholder with an ID not present in the list is assumed to be an argument placeholder with the
   * unknown ID as a name. For instance, one can use `https://example.com/search?q={query}` instead of the
   * longer `https://example.com/search?q={argument name="query"}`.
   */
  const std::vector<QString> m_reservedPlaceholderIds = {"clipboard", "selected", "uuid", "date"};

public:
  struct ParsedPlaceholder {
    QString id;
    std::map<QString, QString> args;
  };

  using UrlPart = std::variant<QString, ParsedPlaceholder>;

  struct Argument {
    QString name;
    QString defaultValue;
  };

private:
  std::vector<ParsedPlaceholder> m_placeholders;
  std::vector<Argument> m_args;
  std::vector<UrlPart> m_parts;
  QString m_raw;
  QString m_id;
  QString m_name;
  QString m_app;
  QString m_icon;
  QDateTime m_createdAt;
  QDateTime m_updatedAt;
  std::optional<QDateTime> m_lastOpenedAt;
  int m_openCount = 0;

  void insertPlaceholder(const ParsedPlaceholder &placeholder);

public:
  const std::vector<ParsedPlaceholder> &placeholders() const;
  const std::vector<Argument> &arguments() const;

  QString id() const;
  QString url() const;

  /**
   * The ID of the application that is configured to open this url.
   */
  QString app() const;
  QString name() const;
  QString icon() const;
  int openCount() const;
  QDateTime createdAt();
  QDateTime updatedAt();
  std::optional<QDateTime> lastOpenedAt();

  std::vector<UrlPart> parts() const;

  void setApp(const QString &app);
  void setName(const QString &name);
  void setIcon(const QString &icon);
  void setId(const QString &id);
  void parseLink(const QString &link);
  void setCreatedAt(const QDateTime &date);
  void setUpdatedAt(const QDateTime &date);
  void setLastOpenedAt(const std::optional<QDateTime> &date);
  void setOpenCount(int openCount);

  Shortcut() {}
};
