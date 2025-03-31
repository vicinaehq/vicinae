#include "extend/model-parser.hpp"
#include "extend/list-model.hpp"
#include "extend/root-detail-model.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>

ModelParser::ModelParser() {}

std::vector<RenderModel> ModelParser::parse(const QJsonArray &views) {
  std::vector<RenderModel> renderedViews;

  renderedViews.reserve(views.size());

  for (const auto &viewTree : views) {
    auto instance = viewTree.toObject();
    auto root = instance.value("root").toObject();
    auto changes = instance.value("changes").toArray();
    auto type = root.value("type").toString();

    if (type == "list") {
      renderedViews.push_back(ListModelParser().parse(root));
      qDebug() << "push list model with";
    } else if (type == "detail") {
      renderedViews.push_back(RootDetailModelParser().parse(root));
    } else {
      renderedViews.push_back(
          InvalidModel{QString("Component of type %1 cannot be used as the root").arg(type)});
    }
  }

  return renderedViews;
}
