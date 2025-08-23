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

    QString k = parts[0];
    QString v = parts[1];

    if (v.startsWith('"')) { v = v.sliced(1); }
    if (v.endsWith('"')) { v = v.sliced(0, v.size() - 1); }

    if (k == "PRETTY_NAME") {
      m_prettyName = v;
    } else if (k == "ID") {
      m_id = v;
    } else if (k == "VERSION_ID") {
      m_version = v;
    }
  }
}
