#include <qstring.h>

class ActionLabel {
  QString _label;

public:
  const QString &label() const { return _label; }

  ActionLabel(const QString &label) : _label(label) {}
};
