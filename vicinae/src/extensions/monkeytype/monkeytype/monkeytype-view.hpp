#include "layout.hpp"
#include "ui/views/base-view.hpp"
#include "ui/monkeytypewriter/monkey-typewriter.hpp"

class MonkeyTypeView : public BaseView {
public:
  bool supportsSearch() const override { return false; }

  MonkeyTypeView() { VStack().margins(20).add(new MonkeyTypeWriter()).center().imbue(this); }
};
