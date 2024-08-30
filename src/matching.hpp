#ifndef MATCHING_HPP
#define MATCHING_HPP

#include "cssparser.hpp"
#include "dom.hpp"
#include <optional>

// Property-value map
typedef unordered_map<string, Value *> PropertyMap;

enum Display { BLOCK, INLINE, NONE };

class MatchedNode {
public:
  NodeBase *dom_node;
  PropertyMap values;
  vector<MatchedNode *> children;

  MatchedNode(NodeBase *dom_node, PropertyMap &values);
  MatchedNode();
  ~MatchedNode();

  optional<Value *> get_value(const string &name) const;
  Display get_display() const;
  string to_string() const;
  static void print(MatchedNode *node);
  static void free_node(MatchedNode *node);
};

MatchedNode *match(NodeBase *dom, Stylesheet &css);

#endif // !MATCHING_HPP
