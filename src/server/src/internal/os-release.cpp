#include "os-release.hpp"
#include <qfile.h>

QString OsRelease::id() const { return m_id; }
QString OsRelease::prettyName() const { return m_prettyName; }
QString OsRelease::version() const { return m_version; }
bool OsRelease::isValid() const { return m_valid; }

OsRelease::OsRelease() {
  QFile file("/etc/os-release");

  if (!file.open(QIODevice::ReadOnly)) return;

  m_valid = true;

  for (const auto &line : file.readAll().split('\n')) {
    auto parts = line.split('=');

    if (parts.size() != 2) continue;

    QString const k = parts[0];
    QString v = parts[1];

    // Values may be quoted with double or single quotes.
    if (v.size() >= 2 && (v.front() == '"' || v.front() == '\'') && v.back() == v.front()) {
      v = v.sliced(1, v.size() - 2);
    }

    if (k == "PRETTY_NAME") {
      m_prettyName = v;
    } else if (k == "ID") {
      m_id = v;
    } else if (k == "VERSION_ID") {
      m_version = v;
    }
  }
}
