#include "extend/node-tree.hpp"
#include <algorithm>
#include <ranges>

void NodeTree::initRoot(const std::string &rootId) {
  m_rootId = rootId;
  if (m_nodes.contains(rootId)) return;

  m_nodes[rootId] = Node{
      .id = rootId,
      .type = "root",
      .props = {},
      .parentId = {},
      .childIds = {},
      .viewIndex = -1,
  };
}

ApplyResult NodeTree::applyOps(std::span<const TaggedOp> ops) {
  ApplyResult result;

  for (const auto &op : ops) {
    if (op.op == "create") {
      m_nodes[op.id] = Node{
          .id = op.id,
          .type = op.type,
          .props = op.props,
          .parentId = {},
          .childIds = {},
          .viewIndex = -1,
      };
    }
  }

  for (const auto &op : ops) {
    if (op.op == "create") {
      if (op.parentId.empty()) continue;
      placeNode(op.id, op.parentId, op.index, result);

    } else if (op.op == "move") {
      auto nodeIt = m_nodes.find(op.id);
      if (nodeIt == m_nodes.end()) continue;

      if (!nodeIt->second.parentId.empty()) {
        auto oldParentIt = m_nodes.find(nodeIt->second.parentId);
        if (oldParentIt != m_nodes.end()) { std::erase(oldParentIt->second.childIds, op.id); }
      }

      nodeIt->second.viewIndex = -1;
      placeNode(op.id, op.parentId, op.index, result);

    } else if (op.op == "update") {
      auto nodeIt = m_nodes.find(op.id);
      if (nodeIt == m_nodes.end()) continue;

      nodeIt->second.props = op.props;

      int vi = resolveViewIndex(op.id);
      if (vi >= 0) {
        result.dirtyViews.insert(vi);
        if (isComponentRoot(op.id)) result.propsDirtyViews.insert(vi);
      }

    } else if (op.op == "remove") {
      int vi = resolveViewIndex(op.id);
      if (vi >= 0) result.dirtyViews.insert(vi);

      auto nodeIt = m_nodes.find(op.id);
      if (nodeIt == m_nodes.end()) continue;

      if (!nodeIt->second.parentId.empty()) {
        auto parentIt = m_nodes.find(nodeIt->second.parentId);
        if (parentIt != m_nodes.end()) { std::erase(parentIt->second.childIds, op.id); }
      }

      removeRecursive(op.id);
    }
  }

  return result;
}

void NodeTree::placeNode(const std::string &nodeId, const std::string &parentId, int index,
                         ApplyResult &result) {
  auto nodeIt = m_nodes.find(nodeId);
  if (nodeIt == m_nodes.end()) return;

  nodeIt->second.parentId = parentId;

  auto parentIt = m_nodes.find(parentId);
  if (parentIt != m_nodes.end()) {
    auto &pc = parentIt->second.childIds;
    auto idx = std::min(static_cast<size_t>(index), pc.size());
    pc.insert(pc.begin() + static_cast<ptrdiff_t>(idx), nodeId);
  }

  int vi = resolveViewIndex(nodeId);
  if (vi >= 0) {
    result.dirtyViews.insert(vi);
    if (isComponentRoot(nodeId)) result.propsDirtyViews.insert(vi);
  }
}

const Node *NodeTree::viewComponentRoot(int viewIndex) const {
  auto rootIt = m_nodes.find(m_rootId);
  if (rootIt == m_nodes.end()) return nullptr;

  const auto &rootChildren = rootIt->second.childIds;
  if (viewIndex < 0 || static_cast<size_t>(viewIndex) >= rootChildren.size()) return nullptr;

  auto wrapperIt = m_nodes.find(rootChildren[viewIndex]);
  if (wrapperIt == m_nodes.end()) return nullptr;

  const auto &wrapperChildren = wrapperIt->second.childIds;
  if (wrapperChildren.empty()) return nullptr;

  auto compIt = m_nodes.find(wrapperChildren.back());
  return compIt != m_nodes.end() ? &compIt->second : nullptr;
}

const Node *NodeTree::node(const std::string &id) const {
  auto it = m_nodes.find(id);
  return it != m_nodes.end() ? &it->second : nullptr;
}

size_t NodeTree::viewCount() const {
  auto rootIt = m_nodes.find(m_rootId);
  if (rootIt == m_nodes.end()) return 0;
  return rootIt->second.childIds.size();
}

int NodeTree::resolveViewIndex(const std::string &nodeId) {
  auto nodeIt = m_nodes.find(nodeId);
  if (nodeIt == m_nodes.end()) return -1;
  if (nodeIt->second.viewIndex >= 0) return nodeIt->second.viewIndex;

  auto *current = &nodeIt->second;
  while (!current->parentId.empty() && current->parentId != m_rootId) {
    auto parentIt = m_nodes.find(current->parentId);
    if (parentIt == m_nodes.end()) return -1;
    current = &parentIt->second;
  }

  if (current->parentId != m_rootId) return -1;

  auto rootIt = m_nodes.find(m_rootId);
  if (rootIt == m_nodes.end()) return -1;

  const auto &rc = rootIt->second.childIds;
  auto pos = std::ranges::find(rc, current->id);
  if (pos == rc.end()) return -1;

  int vi = static_cast<int>(std::distance(rc.begin(), pos));
  nodeIt->second.viewIndex = vi;
  return vi;
}

void NodeTree::removeRecursive(const std::string &id) {
  auto it = m_nodes.find(id);
  if (it == m_nodes.end()) return;

  auto childIds = std::move(it->second.childIds);
  m_nodes.erase(it);

  for (const auto &childId : childIds) {
    removeRecursive(childId);
  }
}

bool NodeTree::isComponentRoot(const std::string &nodeId) const {
  auto nodeIt = m_nodes.find(nodeId);
  if (nodeIt == m_nodes.end()) return false;

  const auto &parentId = nodeIt->second.parentId;
  if (parentId.empty()) return false;

  auto parentIt = m_nodes.find(parentId);
  if (parentIt == m_nodes.end()) return false;
  if (parentIt->second.parentId != m_rootId) return false;

  return parentIt->second.childIds.back() == nodeId;
}
