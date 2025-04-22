#include "app.hpp"
#include "extend/form-model.hpp"
#include "extension/extension-component.hpp"

class ExtensionFormComponent : public AbstractExtensionRootComponent {
public:
  void render(const RenderModel &model) override {
    auto formModel = std::get<FormModel>(model);

    qDebug() << "form model render with" << formModel.items.size() << "items";
  }

  ExtensionFormComponent(AppWindow &app) : AbstractExtensionRootComponent(app) {}
};
