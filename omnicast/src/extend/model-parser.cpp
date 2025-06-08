#include "extend/model-parser.hpp"
#include "extend/form-model.hpp"
#include "extend/grid-model.hpp"
#include "extend/list-model.hpp"
#include "extend/root-detail-model.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qlogging.h>

ModelParser::ModelParser() {}

ParsedRenderData ModelParser::parse(const QJsonArray &views) {
  ParsedRenderData render;

  render.items.reserve(views.size());

  for (const auto &viewTree : views) {
    RenderRoot rootData;
    auto instance = viewTree.toObject();
    auto root = instance.value("root").toObject();
    auto type = root.value("type").toString();

    rootData.dirty = root.value("dirty").toBool(true);
    rootData.propsDirty = root.value("propsDirty").toBool(true);

    if (type == "list") {
      rootData.root = ListModelParser().parse(root);
      // qDebug() << "push list model with";
    } else if (type == "grid") {
      rootData.root = GridModelParser().parse(root);
    } else if (type == "detail") {
      rootData.root = RootDetailModelParser().parse(root);
    } else if (type == "form") {
      rootData.root = FormModel::fromJson(root);
    } else {
      rootData.root = InvalidModel{QString("Component of type %1 cannot be used as the root").arg(type)};
    }

    render.items.emplace_back(rootData);
  }

  return render;
}
