#pragma once
#include <glaze/json/generic.hpp>
#include <glaze/json/read.hpp>
#include <set>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

struct Node {
  std::string id;
  std::string type;
  glz::generic::object_t props;
  std::string parentId;
  std::vector<std::string> childIds;
  int viewIndex = -1;
};

using NodeMap = std::unordered_map<std::string, Node>;

struct TaggedOp {
  std::string op;
  std::string id;
  std::string type;
  std::string parentId;
  int index = 0;
  glz::generic::object_t props;
};

struct ApplyResult {
  std::set<int> dirtyViews;
  std::set<int> propsDirtyViews;
};

class NodeTree {
public:
  void initRoot(const std::string &rootId);

  ApplyResult applyOps(std::span<const TaggedOp> ops);

  const Node *viewComponentRoot(int viewIndex) const;
  const Node *node(const std::string &id) const;
  size_t viewCount() const;
  const std::string &rootId() const { return m_rootId; }

private:
  void placeNode(const std::string &nodeId, const std::string &parentId, int index, ApplyResult &result);
  int resolveViewIndex(const std::string &nodeId);
  void removeRecursive(const std::string &id);
  bool isComponentRoot(const std::string &nodeId) const;

  NodeMap m_nodes;
  std::string m_rootId;
};

template <typename Fn> void forEachChild(const Node &node, const NodeTree &tree, Fn &&fn) {
  for (const auto &childId : node.childIds) {
    if (const auto *child = tree.node(childId)) fn(*child);
  }
}
