#include <qstring.h>

class OsRelease {
  bool m_valid = false;
  QString m_prettyName;
  QString m_id;
  QString m_version;

public:
  bool isValid() const;
  QString prettyName() const;
  QString id() const;
  QString version() const;

  OsRelease();
};
