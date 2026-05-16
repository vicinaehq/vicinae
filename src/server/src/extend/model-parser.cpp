#include "extend/model-parser.hpp"
#include "extend/form-model.hpp"
#include "extend/grid-model.hpp"
#include "extend/list-model.hpp"
#include "extend/root-detail-model.hpp"

ParsedRenderData ModelParser::parse(const NodeTree &tree, const ApplyResult &result) {
  ParsedRenderData render;
  render.items.reserve(tree.viewCount());

  for (size_t i = 0; i < tree.viewCount(); ++i) {
    RenderRoot rootData;
    int vi = static_cast<int>(i);

    rootData.dirty = result.dirtyViews.contains(vi);
    rootData.propsDirty = result.propsDirtyViews.contains(vi);

    if (!rootData.dirty && !rootData.propsDirty) {
      render.items.emplace_back(rootData);
      continue;
    }

    const auto *componentRoot = tree.viewComponentRoot(vi);
    if (!componentRoot) {
      rootData.root = InvalidModel{QString("No component root for view %1").arg(vi)};
      render.items.emplace_back(rootData);
      continue;
    }

    bool childrenDirty = result.dirtyViews.contains(vi);

    if (componentRoot->type == "list") {
      auto m = ListModelParser().parse(*componentRoot, tree);
      m.dirty = childrenDirty;
      rootData.root = std::move(m);
    } else if (componentRoot->type == "grid") {
      auto m = GridModelParser().parse(*componentRoot, tree);
      m.dirty = childrenDirty;
      rootData.root = std::move(m);
    } else if (componentRoot->type == "detail") {
      rootData.root = RootDetailModelParser().parse(*componentRoot, tree);
    } else if (componentRoot->type == "form") {
      rootData.root = FormModel::fromNode(*componentRoot, tree);
    } else {
      rootData.root = InvalidModel{QString("Component of type %1 cannot be used as the root")
                                       .arg(QString::fromStdString(componentRoot->type))};
    }

    render.items.emplace_back(rootData);
  }

  return render;
}
