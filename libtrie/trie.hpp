#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <libqalculate/includes.h>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#define TRIE_LINEAR_CAP 32

template <typename T, typename Hash = std::hash<T>> class Trie {
  // XXX - Implement compression later on
  struct CompressedNode {};

  struct Node {
    struct CharNode {
      uint8_t ch;
      std::unique_ptr<Node> node;

      static CharNode fromChar(uint8_t ch) {
        CharNode node;

        node.ch = ch;
        node.node = std::make_unique<Node>();

        return node;
      }
    };

    using NodeMap = std::unordered_map<uint8_t, std::unique_ptr<Node>>;
    using NodeList = std::vector<CharNode>;
    using Empty = std::monostate;
    using Data = std::variant<Empty, CharNode, NodeList, NodeMap>;

    Data data;
    std::vector<T> matches;
    uint8_t npath = 0;
  };

  size_t memUsage = 0;
  Node m_root;
  bool m_compression;

  /**
   * Tokenizes a string based on:
   * - Whitespace boundaries
   * - Case changes (camelCase -> camel Case)
   * - Non-alphanumeric boundaries
   */
  std::vector<std::string_view> splitWords(std::string_view view) {
    // Define character types for our state machine
    enum TrieCharType { CharTypeLower, CharTypeUpper, CharTypeDigit, CharTypeSpace, CharTypeOther };

    std::vector<std::string_view> words;
    words.reserve(view.size() / 4); // Estimate average word length of 4

    if (view.empty()) { return words; }

    size_t wordStart = 0;
    TrieCharType prevType = CharTypeSpace;

    for (size_t i = 0; i < view.size(); ++i) {
      char ch = view[i];
      TrieCharType currentType;

      // Determine current character type
      if (std::islower(ch))
        currentType = CharTypeLower;
      else if (std::isupper(ch))
        currentType = CharTypeUpper;
      else if (std::isdigit(ch))
        currentType = CharTypeDigit;
      else if (std::isspace(ch))
        currentType = CharTypeSpace;
      else
        currentType = CharTypeOther;

      // Check for word boundaries based on state transitions
      bool isWordBoundary = false;

      if (currentType == CharTypeSpace && prevType == CharTypeSpace) {
        wordStart += 1;
        continue;
      }

      if (currentType == CharTypeSpace) {
        // Space after non-space creates a boundary
        isWordBoundary = (prevType != CharTypeSpace);
      } else if (currentType == CharTypeOther) {
        // Non-alphanumeric creates a boundary
        isWordBoundary = (prevType != CharTypeSpace && prevType != CharTypeOther);
      } else if (currentType == CharTypeUpper && prevType == CharTypeLower) {
        // camelCase transition creates a boundary
        isWordBoundary = true;
      }

      // Handle word boundary
      if (isWordBoundary) {
        // Add word if it has content
        if (i > wordStart) { words.emplace_back(view.substr(wordStart, i - wordStart)); }

        // Skip consecutive spaces or non-alphanumeric chars
        if (currentType == CharTypeSpace || currentType == CharTypeOther) {
          wordStart = i + 1;
        } else {
          wordStart = i;
        }
      } else {
      }

      prevType = currentType;
    }

    // Add the last word if needed
    if (wordStart < view.size()) { words.emplace_back(view.substr(wordStart)); }

    return words;
  }

  const Node *findStartNode(std::string_view prefix) const {
    const Node *cur = &m_root;

    for (char ch : prefix) {
      auto index = static_cast<uint8_t>(tolower(ch));

      if (auto charNode = std::get_if<typename Node::CharNode>(&cur->data)) {
        if (charNode->ch == index) {
          cur = charNode->node.get();
          continue;
        }
      }

      if (auto list = std::get_if<typename Node::NodeList>(&cur->data)) {
        auto it = std::ranges::find_if(*list, [index](const auto &charNode) { return charNode.ch == index; });

        if (it != list->end()) {
          cur = it->node.get();
          continue;
        }
      }

      if (auto map = std::get_if<typename Node::NodeMap>(&cur->data)) {
        if (auto it = map->find(index); it != map->end()) {
          cur = it->second.get();
          continue;
        }
      }

      return {};
    }

    return cur;
  }

  void traversePaths(const Node *node, const std::function<void(const Node *)> &fn) const {
    std::vector<const Node *> paths;

    paths.push_back(node);

    while (!paths.empty()) {
      const Node *node = paths[paths.size() - 1];

      paths.pop_back();
      paths.reserve(paths.size() + node->npath);
      fn(node);

      if (auto charNode = std::get_if<typename Node::CharNode>(&node->data)) {
        paths.push_back(charNode->node.get());
      } else if (auto list = std::get_if<typename Node::NodeList>(&node->data)) {
        for (const auto &charNode : *list) {
          paths.push_back(charNode.node.get());
        }
      } else if (auto map = std::get_if<typename Node::NodeMap>(&node->data)) {
        for (const auto &[ch, node] : *map) {
          paths.push_back(node.get());
        }
      }
    }
  }

public:
  using UnorderedSet = std::unordered_set<T, Hash>;

  Trie() {}

  void clear() { m_root = {}; }

  bool exactMatch(std::string_view query) const {
    const Node *cur = findStartNode(query);

    return cur && !cur->matches.empty();
  }

  /**
   * Call `fn` for each unique item matching the provided prefix.
   * Stops after having collected at least `limit` unique results.
   */
  void prefixTraverse(std::string_view prefix, const std::function<void(const T &)> &fn,
                      int limit = 1000) const {
    const Node *start = findStartNode(prefix);

    if (!start) return;

    std::unordered_set<size_t> m_visited;
    std::vector<const Node *> paths;

    paths.push_back(start);

    while (!paths.empty()) {
      const Node *node = paths[paths.size() - 1];

      paths.pop_back();
      paths.reserve(paths.size() + node->npath);

      for (const auto &match : node->matches) {
        size_t key = Hash()(match);
        bool exists = std::ranges::find(m_visited, key) != m_visited.end();

        if (!exists) {
          fn(match);
          m_visited.insert(key);
          if (m_visited.size() >= limit) return;
        }
      }

      if (auto charNode = std::get_if<typename Node::CharNode>(&node->data)) {
        paths.push_back(charNode->node.get());
      } else if (auto list = std::get_if<typename Node::NodeList>(&node->data)) {
        for (const auto &charNode : *list) {
          paths.push_back(charNode.node.get());
        }
      } else if (auto map = std::get_if<typename Node::NodeMap>(&node->data)) {
        for (const auto &[ch, node] : *map) {
          paths.push_back(node.get());
        }
      }
    }
  }

  /**
   * Erase value from the trie.
   * This operation is quite expensive and only suitable for very sparse removals.
   * If you have a lot to remove, rebuilding the trie anew is probably best.
   */
  void erase(const T &value) {
    traversePaths(m_root, [&](const Node *node) { std::ranges::remove(node->matches, value); });
  }

  std::vector<T> prefixSearch(std::string_view prefix, int limit = 1000) const {
    std::vector<T> items;

    prefixTraverse(prefix, [&](const T &match) { items.emplace_back(match); }, limit);

    return items;
  }

  /**
   * Splits the text into words and then index each one separately.
   */
  void indexLatinText(std::string_view s, const T &data) {
    for (const auto &word : splitWords(s)) {
      index(word, data);
    }
  }

  void index(std::string_view s, const T &data) {
    if (s.empty()) return;

    Node *cur = &m_root;

    for (char ch : s) {
      uint8_t idx = static_cast<uint8_t>(tolower(ch));

      if (std::holds_alternative<std::monostate>(cur->data)) {
        auto charNode = Node::CharNode::fromChar(idx);
        auto ptr = charNode.node.get();

        cur->data = std::move(charNode);
        cur->npath = 1;
        cur = ptr;
        memUsage += sizeof(Node);
      }

      else if (auto charNode = std::get_if<typename Node::CharNode>(&cur->data)) {
        if (idx == charNode->ch) {
          cur = charNode->node.get();
          continue;
        }

        typename Node::NodeList list;
        auto newNode = Node::CharNode::fromChar(idx);
        auto newCur = newNode.node.get();

        list.reserve(2);
        list.emplace_back(std::move(*charNode));
        list.emplace_back(std::move(newNode));
        cur->data = std::move(list);
        cur->npath++;

        cur = newCur;
      }

      else if (auto list = std::get_if<typename Node::NodeList>(&cur->data)) {
        auto it = std::ranges::find_if(*list, [idx](const auto &charNode) { return charNode.ch == idx; });

        if (it != list->end()) {
          cur = it->node.get();
          continue;
        }

        if (list->size() < TRIE_LINEAR_CAP) {
          auto newNode = Node::CharNode::fromChar(idx);
          auto newCur = newNode.node.get();

          list->emplace_back(std::move(newNode));
          cur->npath++;
          cur = newCur;
          continue;
        }

        typename Node::NodeMap map;
        memUsage += sizeof(map);

        auto newNode = std::make_unique<Node>();
        auto nodePtr = newNode.get();

        for (auto &charNode : *list) {
          map[charNode.ch] = std::move(charNode.node);
        }

        map[idx] = std::move(newNode);
        cur->data = std::move(map);
        cur->npath++;
        cur = nodePtr;
        continue;
      }

      else if (auto map = std::get_if<typename Node::NodeMap>(&cur->data)) {
        if (auto it = map->find(idx); it != map->end()) {
          cur = it->second.get();
          continue;
        }

        auto newNode = std::make_unique<Node>();
        auto nodePtr = newNode.get();

        map->insert({idx, std::move(newNode)});
        cur->npath++;
        cur = nodePtr;
      }
    }

    if (std::ranges::find(cur->matches, data) == cur->matches.end()) { cur->matches.emplace_back(data); }

    memUsage += sizeof(T) * cur->matches.capacity();
  }
};
