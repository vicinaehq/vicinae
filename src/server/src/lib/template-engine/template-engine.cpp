#include "template-engine.hpp"
#include "template-engine/template-engine.hpp"

void TemplateEngine::setVar(const QString &key, const QString &value) { m_vars[key] = value; }

std::optional<QString> TemplateEngine::var(const QString &key) const {
  if (auto it = m_vars.find(key); it != m_vars.end()) { return it->second; }

  return std::nullopt;
}

QString TemplateEngine::build(const QString &schema) {
  QString output = schema;

  // inefficient but good enough for now
  for (const auto &[k, v] : m_vars) {
    auto placeholder = QString("{%1}").arg(k);
    output.replace(placeholder, v);
  }

  return output;
}
