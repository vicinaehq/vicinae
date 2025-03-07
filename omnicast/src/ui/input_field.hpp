#include <qlineedit.h>

class InputField : public QLineEdit {
public:
  InputField() { setProperty("class", "form-input"); }
};
